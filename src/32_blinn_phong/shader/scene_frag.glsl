#version 330 core
out vec4 FragColor;

in vec2 outTexCoord;
in vec3 outNormal;
in vec3 outFragPos;
// 点光源
struct PointLight {
  vec3 position;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform sampler2D woodMap; // 贴图
uniform PointLight light; //点光源
uniform vec3 viewPos;   // 眼睛位置
uniform bool blinn;

void main() 
{
    vec3 color = texture(woodMap, outTexCoord).rgb; //物体颜色
    vec3 viewDir = normalize(viewPos - outFragPos); //观察向量
    vec3 normal = normalize(outNormal); //法线
    vec3 lightDir = normalize(light.position - outFragPos);//光线向量
  
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);

    // 镜面光着色
    float spec = 0.0;
    if(blinn) 
    {
      vec3 halfwayDir = normalize(lightDir + viewDir);
      spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);

    } else 
    {
      vec3 reflectDir = reflect(-lightDir, normal);
      spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }

    // 衰减
    float distance = length(light.position - outFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +light.quadratic * (distance * distance));  

    // 合并结果
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * color;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
}