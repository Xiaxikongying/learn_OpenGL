#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 outFragPos;  //����λ��
out vec3 outNormal;   //������
out vec2 outTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0f);
    //�����ת ���ȱ����ŶԷ��ߵ�Ӱ��
    outNormal = mat3(transpose(inverse(model))) * aNormal;;
    outFragPos = vec3(model * vec4(aPosition, 1.0f));
    outTexCoord = aTexCoords;
}