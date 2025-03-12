#version 330 core
out vec4 FragColor;
in vec2 outTexCoord;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;
uniform float exposure;

void main() 
{
  const float gamma = 2.2;
  vec3 hdrColor = texture(scene, outTexCoord).rgb; //正常颜色
  vec3 bloomColor = texture(bloomBlur, outTexCoord).rgb;//模糊后的颜色
  if(bloom)
  {
    hdrColor += bloomColor; 
  }
  vec3 result = vec3(1.0) - exp(-hdrColor * exposure);// exp =  e^(1/hdrColor * exposure)  
  result = pow(result, vec3(1.0 / gamma));
  FragColor = vec4(hdrColor, 1.0);
}