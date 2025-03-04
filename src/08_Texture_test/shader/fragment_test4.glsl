#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord; //��������

uniform sampler2D Texture1; //����ͼ��id  sampler2d�ǲ����������ʾ�������
uniform sampler2D Texture2;
uniform float mixValue;

void main()
{
    FragColor = mix(texture(Texture1, TexCoord), texture(Texture2, TexCoord), mixValue); //���Բ�ֵ�������� 
}