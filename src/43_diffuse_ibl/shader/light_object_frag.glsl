#version 330 core
layout(location = 0) out vec4 FragColor;
uniform vec3 lightColor;
float exposure = 1.0f;
void main() 
{
  vec3 color = lightColor / 200;
  //HDR
  color = vec3(1.0) - exp(-color * exposure); 
  color = pow(color, vec3(1.0 / 2.2));
  FragColor = vec4(color, 1.0);
}