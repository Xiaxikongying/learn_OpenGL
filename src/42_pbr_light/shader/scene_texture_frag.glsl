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
//获取法线（世界空间）
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
//法线分布函数（NDF): 描述表面微平面的方向分布
float DistributionGGX(vec3 N, vec3 H, float roughness) //NDF
{
  float a = roughness * roughness; //粗糙度?
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
//几何遮蔽（G）: 计算自遮挡和自阴影
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) //G
{
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  float ggx1 = GeometrySchlickGGX(NdotL, roughness);
  return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
//Fresnel 反射（F）: 计算表面角度对反射率的影响
vec3 fresnelSchlick(float cosTheta, vec3 F0) //F
{
  return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
void main() 
{
  //从贴图中获取对应的值
  vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));//反照率
  float metallic = texture(metallicMap, TexCoords).r;//金属度
  float roughness = texture(roughnessMap, TexCoords).r;//粗糙程度
  float ao = texture(aoMap, TexCoords).r;//环境光遮蔽

  vec3 N = getNormalFromMap(); //法线向量
  vec3 V = normalize(camPos - WorldPos); //观察向量

  //0°入射角的反射率
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic); //F0会因为金属度而有改变

  // 计算四个光源的总和光照
  vec3 Lo = vec3(0.0);
  for(int i = 0; i < 4; ++i) 
  {
    vec3 L = normalize(lightPositions[i] - WorldPos); //光源方向
    vec3 H = normalize(V + L);//半程向量
    float distance = length(lightPositions[i] - WorldPos);//物体与光源的距离
    float attenuation = 1.0 / (distance * distance);//衰减值  与dis
    vec3 radiance = lightColors[i] * attenuation; //衰减后的光强度

    // 计算DGF的值
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F; //DFG
    //4*(n.w)(n.v)   + 0.0001是为了防止除零
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; 
    vec3 specular = numerator / denominator; //计算镜面反射

    vec3 kS = F; //镜面反射系数
    vec3 kD = vec3(1.0) - kS;//漫反射系数
    kD *= 1.0 - metallic;	  //金属性越大，漫反射系数越小 （金属表面不会散射光）
    float NdotL = max(dot(N, L), 0.0);//光源方向与法向量的夹角 cos
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;  
  }

  vec3 ambient = vec3(0.03) * albedo * ao; //环境光强设置为0.03
  vec3 color = ambient + Lo;


  //color = color / (color + vec3(1.0)); //hdr
  color = vec3(1.0) - exp(-color * exposure); 
  color = pow(color, vec3(1.0 / 2.2));//gamma

  FragColor = vec4(color, 1.0);
}