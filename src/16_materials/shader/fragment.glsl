#version 330 core
out vec4 FragColor;

in vec3 outFragPos;  //����λ��
in vec3 outNormal;   //������

// ������ʽṹ��
struct Material
{
    vec3 ambient; // ����Ի�����ķ���
    vec3 diffuse; // ������������ķ���
    vec3 specular; // ����Ը߹���ɫ�ķ���
    float shininess; // ����ĸ߹�ָ��
};
// ��Դ����
struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 objectColor;  //������ɫ
uniform vec3 viewPos;  //�۲�λ��

void main()
{
    //������
    vec3 ambient = light.ambient * material.ambient;

    //������⣺diffuse
    vec3 norm = normalize(outNormal);
    vec3 lightDir = normalize(light.position - outFragPos); //��Դ����
    float diff = max(dot(norm, lightDir), 0.0);     //����� �� ���� �ļн�
    vec3 diffuse = light.diffuse * diff * material.diffuse; // ������

    //���淴��⣺specular
    vec3 viewDir = normalize(viewPos - outFragPos); //�۲ⷽ��
    vec3 reflectDir = reflect(-lightDir, norm);     //���䷽��
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);  //����ⷽ�� �� �۲ⷽ�� �ļн�  
    vec3 specular = light.specular * spec * material.specular;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}