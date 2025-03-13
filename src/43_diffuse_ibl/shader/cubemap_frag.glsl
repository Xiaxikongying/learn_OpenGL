#version 330 core
layout(location = 0) out vec4 FragColor;

uniform sampler2D equireMap; //hdr贴图


in vec3 localPos;
const vec2 invAtan = vec2(0.1591, 0.3183); //角度转换常量  1/π  1/2π

vec2 SampleSphericalMap(vec3 v) 
{
  vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
  uv *= invAtan;
  uv += 0.5;
  return uv;
}
//将等距柱状投影的 HDR贴图 转换为立方体贴图
void main() 
{
  vec2 uv = SampleSphericalMap(normalize(localPos));
  vec3 color = texture(equireMap, uv).rgb;
  FragColor = vec4(color, 1.0);
}