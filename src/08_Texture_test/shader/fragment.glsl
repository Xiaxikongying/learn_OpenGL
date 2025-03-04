#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord; //��������

uniform sampler2D Texture1; //����ͼ��id  sampler2d�ǲ����������ʾ�������
uniform sampler2D Texture2;

void main()
{
    //FragColor = texture(Texture1, TexCoord);  //texture  ������ͼ����ȡ����Ӧ�������ɫ
    //FragColor = texture(Texture1, TexCoord) * vec4(ourColor, 1.0);
    FragColor = mix(texture(Texture1, TexCoord), texture(Texture2, TexCoord), 0.2); //���Բ�ֵ�������� 
    //������������1��ʾ���ص�һ������  0��ʾ���صڶ�������
}