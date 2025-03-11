#version 330 core
out vec4 FragColor;

in VS_OUT 
{
  vec3 FragPos;
  vec3 Normal;
  vec2 TexCoords;
  vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) 
{
  // 执行透视除法
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  // 变换到[0,1]的范围
  projCoords = projCoords * 0.5 + 0.5;
  // 取得当前像素的最小的深度
  float closestDepth = texture(shadowMap, projCoords.xy).r;  
  // 取得当前片段在光源视角下的深度
  float currentDepth = projCoords.z;

  //如果当前深度大于 当前像素点的最小深度，说明当前顶点是在阴影中
  // float shadow = currentDepth > closestDepth  ? 1.0 : 0.0; 

  // 纹理偏移  由于浮点精度误差，表面自身的 currentDepth 可能会比 closestDepth 稍微大一点，
  //导致它被错误地判定为 阴影区域
  float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
  // // 检查当前片段是否在阴影中
  // float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

  // PCF 柔和阴影，在 depthMap 周围采样多个深度值，取平均：
  float shadow = 0.0;
  vec2 texSize = 1.0 / textureSize(shadowMap, 0);
  for(int i = -1; i <= 1; i++) 
  {
    for(int j = -1; j <= 1; j++) 
    {
      float pcfDepth = texture(shadowMap, projCoords.xy + vec2(i, j) * texSize).r;
      shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  shadow /= 9.0;

  // 解决cl_clamp_to_border不起作用问题
  if(projCoords.z > 1.0) 
  {
    shadow = 0.0;
  }

  return shadow;
}

void main() 
{
  vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;//获取物体贴图颜色
  vec3 normal = normalize(fs_in.Normal);
  vec3 lightColor = vec3(1.0);
  // ambient
  vec3 ambient = 0.3 * lightColor;
  // diffuse
  vec3 lightDir = normalize(lightPos - fs_in.FragPos);
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = diff * lightColor;
  // specular
  vec3 viewDir = normalize(viewPos - fs_in.FragPos);
  float spec = 0.0;
  vec3 halfwayDir = normalize(lightDir + viewDir);
  spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
  vec3 specular = spec * lightColor;    

  // calculate shadow
  float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
  vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

  FragColor = vec4(lighting, 1.0);
}