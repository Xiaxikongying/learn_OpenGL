#version 330 core
out vec4 FragColor;
in vec2 outTexCoord;

uniform sampler2D Texture1;
uniform sampler2D Texture2;

void main() 
{

  FragColor = mix(texture(Texture1, outTexCoord), texture(Texture2, outTexCoord), 0.1);
  // FragColor = vec4(1.0, 0.6, 0.1, 0.3);

  // //gl_PointCoord 是 GLSL 内置变量，用于点的片元着色器中
  // vec2 coord = gl_PointCoord - vec2(0.5);  //coord 表示的是每个像素相对于点的中心的偏移量。
  // float r = float(length(coord) > 0.5);   //计算当前像素 是否在半径为 0.5 的圆形范围内。

  // //如果 r = 1.0（在圆外），这个像素会被完全透明
  // //如果 r = 0.0（在圆内），这个像素会显示颜色
  // FragColor = vec4(0.0, 0.91, 0.9, (1 - r) * 0.1); 
}