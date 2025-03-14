#version 330 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;

out vec3 worldPos;

uniform mat4 view;
uniform mat4 projection;

//绘制天空盒子
void main() 
{
  worldPos = Position; 
  mat4 rotView = mat4(mat3(view)); //去掉视图矩阵的平移部分，相机的位置不影响天空盒。
  vec4 clipPos = projection * rotView * vec4(worldPos, 1.0);
  gl_Position = clipPos.xyww;
}