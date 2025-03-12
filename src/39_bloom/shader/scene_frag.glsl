#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

// �����
struct DirectionLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
// ���Դ
struct PointLight {
  vec3 position;
  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
// �۹��
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
uniform sampler2D Texture; // ��ͼ
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
  // �������
  vec3 result = CalcDirectionLight(directionLight, normal, viewDir);
  // ���Դ
  for(int i = 0; i < NR_POINT_LIGHTS; i++) 
  {
    result += CalcPointLight(pointLights[i], normal, outFragPos, viewDir);
  }
  vec4 color = vec4(result, 1.0) * texture(Texture, outTexCoord); //����͸���ȵļ���
  
  
  //BrightColorֻ�ռ������Ĺ�Դ
  float brightness = dot(vec3(color.rgb), vec3(0.2126, 0.7152, 0.0722));
  if(brightness > 1.0)//�����ȳ��ԻҶ��Ժ��Ǵ���1  
    BrightColor = vec4(color);
  else
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

  FragColor = vec4(color);
}

// ���㶨���
vec3 CalcDirectionLight(DirectionLight light, vec3 normal, vec3 viewDir) 
{
  vec3 lightDir = normalize(light.direction);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

  // �ϲ�
  vec3 ambient = light.ambient;
  vec3 diffuse = light.diffuse * diff;
  vec3 specular = light.specular * spec;

  return ambient + diffuse + specular;
}

// ������Դ
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) 
{
  vec3 lightDir = normalize(light.position - fragPos);
    // ��������ɫ
  float diff = max(dot(normal, lightDir), 0.0);
    // �������ɫ
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // ˥��
  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));    
    // �ϲ����
  vec3 ambient = light.ambient;
  vec3 diffuse = light.diffuse * diff;
  vec3 specular = light.specular * spec;
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}

