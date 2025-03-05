#version 330 core
out vec4 FragColor;
in vec2 outTexCoord;

uniform sampler2D Texture1;
uniform sampler2D Texture2;

void main() 
{

  FragColor = mix(texture(Texture1, outTexCoord), texture(Texture2, outTexCoord), 0.1);
  // FragColor = vec4(1.0, 0.6, 0.1, 0.3);

  // //gl_PointCoord �� GLSL ���ñ��������ڵ��ƬԪ��ɫ����
  // vec2 coord = gl_PointCoord - vec2(0.5);  //coord ��ʾ����ÿ����������ڵ�����ĵ�ƫ������
  // float r = float(length(coord) > 0.5);   //���㵱ǰ���� �Ƿ��ڰ뾶Ϊ 0.5 ��Բ�η�Χ�ڡ�

  // //��� r = 1.0����Բ�⣩��������ػᱻ��ȫ͸��
  // //��� r = 0.0����Բ�ڣ���������ػ���ʾ��ɫ
  // FragColor = vec4(0.0, 0.91, 0.9, (1 - r) * 0.1); 
}