#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord; //��������

uniform sampler2D Texture1; //����ͼ��id  sampler2d�ǲ����������ʾ�������
uniform sampler2D Texture2;

void main()
{
    //1.0-TexCoord.x�ı�Ц������
    FragColor = mix(texture(Texture1, TexCoord), texture(Texture2, vec2(1.0 - TexCoord.x, TexCoord.y)), 0.2); //���Բ�ֵ�������� 
}