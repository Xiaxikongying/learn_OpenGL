#version 330 core
out vec4 FragColor;

in vec2 outTexCoord;
uniform sampler2D image; //����������
uniform bool horizontal; //true-ˮƽģ��;  false-��ֱģ��

//��˹��Ȩ��
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main() 
{
  vec2 tex_offset = 1.0 / textureSize(image, 0); // ��ȡ�������صĴ�С
  vec3 result = texture(image, outTexCoord).rgb * weight[0]; //��ǰ������ɫ�� ����Ȩ��weight[0]
  if(horizontal) //����Ǵ�ֱģ��
  {
    for(int i = 1; i < 5; ++i) 
    {
      //�����Ҳ����ĸ�����
      result += texture(image, outTexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
      result += texture(image, outTexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
    }
  } 
  else //�����ˮƽģ��
  {
    for(int i = 1; i < 5; ++i) 
    {
      //�����²����ĸ�����
      result += texture(image, outTexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
      result += texture(image, outTexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
    }
  }
  //����ģ�������ɫ
  FragColor = vec4(result, 1.0);
}