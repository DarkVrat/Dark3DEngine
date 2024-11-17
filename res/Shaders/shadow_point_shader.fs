#version 330 core
out vec4 FragColor;

in vec3 FragPos;  
in vec3 ViewPos;
in vec2 TexCoords;
in mat3 TBN;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_height1;
    float     shininess;
}; 
uniform Material material;

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define NR_POINT_LIGHTS 1
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform samplerCube depthMap;
uniform float far_plane;
uniform bool shadows;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords);

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - pointLights[0].position;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(ViewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples); 
        
    return shadow;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
	float heightScale = 0.01;
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(material.texture_height1, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(material.texture_height1, currentTexCoords).r;  
        currentLayerDepth += layerDepth;  
    }
    
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(material.texture_height1, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;  
} 

void main()
{   
	mat3 tTBN = transpose(TBN);
	vec3 viewDir = normalize(tTBN * ViewPos - tTBN * FragPos);
	vec2 texCoords = ParallaxMapping(TexCoords,  viewDir);
	viewDir = normalize(ViewPos - FragPos);
	
    vec3 normal = texture(material.texture_normal1, texCoords).rgb;
	normal = normal * 2.0 - 1.0;   
	normal = normalize(TBN * normal); 

    vec3 result = CalcPointLight(pointLights[0], normal, FragPos, viewDir, texCoords);    
    
    gl_FragColor = vec4(result, texture(material.texture_diffuse1, texCoords).a);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir  = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess*4);

    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
	float shadow = shadows ? ShadowCalculation(FragPos) : 0.0;  
	
    return (ambient + (1.0 - shadow)*(diffuse + specular));
} 
