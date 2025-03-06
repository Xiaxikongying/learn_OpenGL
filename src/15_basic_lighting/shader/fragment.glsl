#version 330 core
out vec4 FragColor;

in vec3 outFragPos;  //����λ��
in vec3 outNormal;   //������

uniform vec3 objectColor;  //������ɫ
uniform vec3 lightColor;   //�ƹ���ɫ
uniform vec3 lightPos;  //��Դλ��
uniform vec3 viewPos;  //�۲�λ��
uniform float ambientStrength; //������ǿ��
uniform float diffuseStrength; //������ǿ��
uniform float specularStrength; //���淴��ǿ�� (������Ĺ⻬�̶�)


void main()
{
    //������
    vec3 ambient = ambientStrength * lightColor; 

    //������⣺diffuse
    vec3 norm = normalize(outNormal);
    vec3 lightDir = normalize(lightPos - outFragPos); //��Դ����
    float diff = max(dot(norm, lightDir), 0.0);     //����� �� ���� �ļн�
    vec3 diffuse = diff * lightColor;

    //���淴��⣺specular
    vec3 viewDir = normalize(viewPos - outFragPos); //�۲ⷽ��
    vec3 reflectDir = reflect(-lightDir, norm);     //���䷽��
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);  //����ⷽ�� �� �۲ⷽ�� �ļн�  32��ʾ�߹ⷶΧ
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}