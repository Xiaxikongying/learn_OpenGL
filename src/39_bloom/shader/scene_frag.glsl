#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

// 定向光
struct DirectionLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
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
// 聚光灯
struct SpotLight {
  vec3 position;
  vec3 direction;
  float cutOff;
  float outerCutOff;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

#define NR_POINT_LIGHTS 4

uniform DirectionLight directionLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform sampler2D Texture; // 贴图
uniform vec3 viewPos;

in vec2 outTexCoord;
in vec3 outNormal;
in vec3 outFragPos;

vec3 CalcDirectionLight(DirectionLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() 
{
  vec3 viewDir = normalize(viewPos - outFragPos);
  vec3 normal = normalize(outNormal);
  // 定向光照
  vec3 result = CalcDirectionLight(directionLight, normal, viewDir);
  // 点光源
  for(int i = 0; i < NR_POINT_LIGHTS; i++) 
  {
    result += CalcPointLight(pointLights[i], normal, outFragPos, viewDir);
  }
  vec4 color = vec4(result, 1.0) * texture(Texture, outTexCoord); //加上透明度的计算
  
  
  //BrightColor只收集明亮的光源
  float brightness = dot(vec3(color.rgb), vec3(0.2126, 0.7152, 0.0722));
  if(brightness > 1.0)//若亮度乘以灰度以后还是大于1  
    BrightColor = vec4(color);
  else
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

  FragColor = vec4(color);
}

// 计算定向光
vec3 CalcDirectionLight(DirectionLight light, vec3 normal, vec3 viewDir) 
{
  vec3 lightDir = normalize(light.direction);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

  // 合并
  vec3 ambient = light.ambient;
  vec3 diffuse = light.diffuse * diff;
  vec3 specular = light.specular * spec;

  return ambient + diffuse + specular;
}

// 计算点光源
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) 
{
  vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
  float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // 衰减
  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));    
    // 合并结果
  vec3 ambient = light.ambient;
  vec3 diffuse = light.diffuse * diff;
  vec3 specular = light.specular * spec;
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}

