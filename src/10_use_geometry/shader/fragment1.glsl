#version 330 core
out vec4 FragColor;
in vec2 outTexCoord;

uniform sampler2D Texture1;
uniform sampler2D Texture2;

void main() 
{
  FragColor = texture(Texture1, outTexCoord);
}