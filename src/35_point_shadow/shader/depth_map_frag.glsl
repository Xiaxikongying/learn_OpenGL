#version 330 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane; //远平面距离

void main() 
{
    // 获取片段和光源之间的距离
    float lightDistance = length(FragPos.xyz - lightPos);
    // 通过除以far_plane映射到[0 - 1]范围
    lightDistance = lightDistance / far_plane;
    // 修改深度值  即修改深度贴图
    gl_FragDepth = lightDistance;
}