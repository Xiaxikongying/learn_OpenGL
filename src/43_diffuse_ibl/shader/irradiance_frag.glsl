#version 330 core
layout(location = 0) out vec4 FragColor;

in vec3 worldPos;
uniform samplerCube envMap;//天空盒贴图

const float PI = 3.14159265359;
void main() 
{
  vec3 N = normalize(worldPos);//可以将盒子坐标视为在天空盒中的法线向量
  vec3 irradiance = vec3(0.0);//该点的光照强度

  // 从原点计算切线空间
  vec3 up = vec3(0.0, 1.0, 0.0);
  vec3 right = normalize(cross(up, N));
  up = normalize(cross(N, right));

  //在半球上进行采样
  float sampleDelta = 0.025;//采样间隔
  float nrSamples = 0.0;
  for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) //[0-2π]
  {
    for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) //[0-π/2]
    {
      // 笛卡尔球面坐标 (切线空间中)
      vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
      // 切线空间转换到世界空间
      vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
      irradiance += texture(envMap, sampleVec).rgb * cos(theta) * sin(theta);
      nrSamples++;
    }
  }
  irradiance = PI * irradiance * (1.0 / float(nrSamples));
  FragColor = vec4(irradiance, 1.0);

}