#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
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

struct Light {
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
    float Radius;
};

const int NR_LIGHTS = 100;
uniform Light lights[NR_LIGHTS];

// Функция для параллакса
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
    // Вычисление viewDir в системе координат TBN
    mat3 tTBN = transpose(TBN);
    vec3 viewDir = normalize(tTBN * (ViewPos - FragPos));

    // Применение Parallax Mapping для корректировки координат текстуры
    vec2 correctedTexCoords = ParallaxMapping(TexCoords, viewDir);

    // Получение данных о текстурах с учётом скорректированных координат
    vec3 albedo = texture(material.texture_diffuse1, correctedTexCoords).rgb;
    float specular = texture(material.texture_specular1, correctedTexCoords).r;
    vec3 normal = texture(material.texture_normal1, correctedTexCoords).rgb;
    normal = normalize(TBN * (normal * 2.0 - 1.0));

    // Базовое освещение
    vec3 lighting = albedo * 0.1;

    // Позиция камеры
    vec3 viewDirWorld = normalize(ViewPos - FragPos);

    // Цикл освещения
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        float distance = length(lights[i].Position - FragPos);
        if(distance < lights[i].Radius)
        {
            vec3 lightDir = normalize(lights[i].Position - FragPos);
            vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * lights[i].Color;
            vec3 halfwayDir = normalize(lightDir + viewDirWorld);  
            float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
            vec3 specularLight = lights[i].Color * spec * specular;

            // Аттенюация (затухание света)
            float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
            diffuse *= attenuation;
            specularLight *= attenuation;
            
            lighting += diffuse + specularLight;
        }
    }

    // Финальный цвет фрагмента
    FragColor = vec4(lighting, 1.0);
}