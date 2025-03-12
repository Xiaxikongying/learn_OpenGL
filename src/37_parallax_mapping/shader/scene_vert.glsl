#version 330 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;
layout(location = 3) in vec3 Tangent; // 切线
layout(location = 4) in vec3 Bitangent; // 副切线

out VS_OUT 
{
  vec3 FragPos;//顶点位置
  vec2 TexCoords;//纹理坐标
  vec3 TangentLightPos;//切线空间光源位置
  vec3 TangentViewPos;//切线空间观察位置
  vec3 TangentFragPos;//切线空间顶点位置
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uvScale;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {

  gl_Position = projection * view * model * vec4(Position, 1.0f);
  vs_out.FragPos = vec3(model * vec4(Position, 1.0));
  vs_out.TexCoords = TexCoords * uvScale;

  mat3 normalMatrix = transpose(inverse(mat3(model)));
  vec3 T = normalize(normalMatrix * Tangent);
  vec3 B = normalize(normalMatrix * Bitangent);
  vec3 N = normalize(normalMatrix * Normal);
  mat3 TBN = transpose(mat3(T, B, N));

  vs_out.TangentLightPos = TBN * lightPos;
  vs_out.TangentViewPos = TBN * viewPos;
  vs_out.TangentFragPos = TBN * vs_out.FragPos;

}