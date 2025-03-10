#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 outFragPos;  //物体位置
out vec3 outNormal;   //法向量
out vec2 outTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0f);
    //解决旋转 不等比缩放对法线的影响
    outNormal = mat3(transpose(inverse(model))) * aNormal;;
    outFragPos = vec3(model * vec4(aPosition, 1.0f));
    outTexCoord = aTexCoords;
}