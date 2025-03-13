#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 camPos;
const float PI = 3.14159265359;
float exposure = 1.0f;

// ----------------------------------------------------------------------------
//��ȡ���ߣ�����ռ䣩
vec3 getNormalFromMap() 
{
  vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

  vec3 Q1 = dFdx(WorldPos);
  vec3 Q2 = dFdy(WorldPos);
  vec2 st1 = dFdx(TexCoords);
  vec2 st2 = dFdy(TexCoords);

  vec3 N = normalize(Normal);
  vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
  vec3 B = -normalize(cross(N, T));
  mat3 TBN = mat3(T, B, N);

  return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
//���߷ֲ�������NDF): ��������΢ƽ��ķ���ֲ�
float DistributionGGX(vec3 N, vec3 H, float roughness) //NDF
{
  float a = roughness * roughness; //�ֲڶ�?
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float nom = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness) 
{
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;

  float nom = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return nom / denom;
}
// ----------------------------------------------------------------------------
//�����ڱΣ�G��: �������ڵ�������Ӱ
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) //G
{
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  float ggx1 = GeometrySchlickGGX(NdotL, roughness);
  return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
//Fresnel ���䣨F��: �������ǶȶԷ����ʵ�Ӱ��
vec3 fresnelSchlick(float cosTheta, vec3 F0) //F
{
  return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
void main() 
{
  //����ͼ�л�ȡ��Ӧ��ֵ
  vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));//������
  float metallic = texture(metallicMap, TexCoords).r;//������
  float roughness = texture(roughnessMap, TexCoords).r;//�ֲڳ̶�
  float ao = texture(aoMap, TexCoords).r;//�������ڱ�

  vec3 N = getNormalFromMap(); //��������
  vec3 V = normalize(camPos - WorldPos); //�۲�����

  //0������ǵķ�����
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic); //F0����Ϊ�����ȶ��иı�

  // �����ĸ���Դ���ܺ͹���
  vec3 Lo = vec3(0.0);
  for(int i = 0; i < 4; ++i) 
  {
    vec3 L = normalize(lightPositions[i] - WorldPos); //��Դ����
    vec3 H = normalize(V + L);//�������
    float distance = length(lightPositions[i] - WorldPos);//�������Դ�ľ���
    float attenuation = 1.0 / (distance * distance);//˥��ֵ  ��dis
    vec3 radiance = lightColors[i] * attenuation; //˥����Ĺ�ǿ��

    // ����DGF��ֵ
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F; //DFG
    //4*(n.w)(n.v)   + 0.0001��Ϊ�˷�ֹ����
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; 
    vec3 specular = numerator / denominator; //���㾵�淴��

    vec3 kS = F; //���淴��ϵ��
    vec3 kD = vec3(1.0) - kS;//������ϵ��
    kD *= 1.0 - metallic;	  //������Խ��������ϵ��ԽС ���������治��ɢ��⣩
    float NdotL = max(dot(N, L), 0.0);//��Դ�����뷨�����ļн� cos
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;  
  }

  vec3 ambient = vec3(0.03) * albedo * ao; //������ǿ����Ϊ0.03
  vec3 color = ambient + Lo;


  //color = color / (color + vec3(1.0)); //hdr
  color = vec3(1.0) - exp(-color * exposure); 
  color = pow(color, vec3(1.0 / 2.2));//gamma

  FragColor = vec4(color, 1.0);
}