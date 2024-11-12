#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec3 ViewPos;
in vec2 TexCoords;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
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
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{   
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(ViewPos - FragPos);

    vec3 result = CalcPointLight(pointLights[0], norm, FragPos, viewDir);    
    
    gl_FragColor = vec4(result, texture(material.texture_diffuse1, TexCoords).a);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir  = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess*4);

    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
	
    return (ambient + diffuse + specular);
} 

