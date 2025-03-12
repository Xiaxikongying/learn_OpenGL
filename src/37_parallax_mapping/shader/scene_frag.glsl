#version 330 core
out vec4 FragColor;

in VS_OUT {
  vec3 FragPos;
  vec2 TexCoords;
  vec3 TangentLightPos;
  vec3 TangentViewPos;
  vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap; // 漫反射贴图
uniform sampler2D normalMap;  // 法线贴图
uniform sampler2D depthMap;   // 高度贴图

uniform float strength;
uniform bool parallax;
uniform float height_scale;

vec2 ParallaxMapping2(vec2 texCoords, vec3 viewDir) {
  // 计算层数  观察角度越大（越倾斜），numLayers 越多，保证平滑过渡
  const float minLayers = 10;
  const float maxLayers = 20;
  float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
  //每层的深度
  float layerDepth = 1.0 / numLayers;
  // 计算每层的 UV 偏移
  vec2 P = viewDir.xy / viewDir.z * height_scale;
  vec2 deltaTexCoords = P / numLayers;

  // 初始UV位置-->texCoords
  vec2 currentTexCoords = texCoords;
  float currentDepthMapValue = texture(depthMap, currentTexCoords).r;

  // 当前累计的层深度
  float currentLayerDepth = 0.0;
  while(currentLayerDepth < currentDepthMapValue) 
  {
    currentTexCoords -= deltaTexCoords;  // 纹理坐标按照 P 方向偏移
    currentDepthMapValue = texture(depthMap, currentTexCoords).r;   // 读取高度图的新值
    currentLayerDepth += layerDepth;  // 累计当前层深度
  }
  // 获取上一层的纹理坐标（回退一层）
  vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

  // 获取上一层和当前层的深度误差
  float afterDepth = currentDepthMapValue - currentLayerDepth;
  float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
  
  // 计算插值权重
  float weight = afterDepth / (afterDepth - beforeDepth);
  vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

  return finalTexCoords;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    float height =  texture(depthMap, texCoords).r;    
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return texCoords - p;    
}

void main() {

  float gamma = 2.2;
  vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
  vec2 texCoords = fs_in.TexCoords;
  if(parallax) 
  {
    texCoords = ParallaxMapping(fs_in.TexCoords, viewDir);
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
      discard;
  }

  vec3 color = pow(texture(diffuseMap, texCoords).rgb, vec3(gamma));
  vec3 normal = texture(normalMap, texCoords).rgb;
  // 将法线向量转换到[-1，1]范围
  normal = normalize(normal * 2.0 - 1.0);
  vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
 
  // 环境光
  vec3 ambient = strength * color; 
  // 漫反射
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = diff * color; 
  //镜面反射
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
  vec3 specular = vec3(0.3) * spec;

  vec3 result = (ambient + diffuse + specular);
  FragColor = vec4(result, 1.0);
  FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));

}

