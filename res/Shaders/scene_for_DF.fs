#version 330 core
out vec4 gPosition;
out vec4 gNormal;
out vec4 gAlbedoSpec;

in vec3 FragPos;  
in vec2 TexCoords;
in mat3 TBN;
in vec3 ViewPos;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_height1;
    float shininess;
}; 
uniform Material material;

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
  
    vec2 currentTexCoords = texCoords;
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
    vec3 viewDir = normalize(tTBN * (ViewPos - FragPos));
    vec2 correctedTexCoords = ParallaxMapping(TexCoords, viewDir);
    vec3 normal = texture(material.texture_normal1, correctedTexCoords).rgb;
    normal = normalize(TBN * (normal * 2.0 - 1.0)); 

    gPosition = vec4(FragPos, 1.0);
    gNormal = vec4(normal, 1.0);
    
    vec3 albedo = texture(material.texture_diffuse1, correctedTexCoords).rgb;
    float specular = texture(material.texture_specular1, correctedTexCoords).r;
    gAlbedoSpec = vec4(albedo, specular);
}