#version 330 core
out vec4 FragColor;
in vec2 outTexCoord;
in vec3 outNormal;
in vec3 outFragPos;


// ������ʽṹ��
struct Material 
{
    sampler2D diffuse; // ��������ͼ
    sampler2D specular; // �������ͼ
    float shininess; // �߹�ָ��
};
// ��Դ����
struct Light 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 lightPos;
uniform vec3 viewPos; // �۾�λ��
uniform vec3 objectColor;  //������ɫ


void main() 
{
    vec3 diffuseTexture = vec3(texture(material.diffuse, outTexCoord));//��������ͼ
    vec3 specularTexture = vec3(texture(material.specular, outTexCoord));//��������ͼ

    // ������
    vec3 ambient = light.ambient * diffuseTexture; 

    // ������
    vec3 norm = normalize(outNormal);
    vec3 lightDir = normalize(lightPos - outFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseTexture; 

    // ���淴��
    vec3 viewDir = normalize(viewPos - outFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * specularTexture; 
    vec3 result = (ambient + diffuse + specular) * vec3(objectColor);

    FragColor = vec4(result, 1.0);
}