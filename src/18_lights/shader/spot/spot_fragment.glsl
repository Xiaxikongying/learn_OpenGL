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
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3  direction; //���շ���
    float cutOff;    //�й��   �ڴ˷�Χ�� ��ǿΪ1
    float outerCutOff; // ���й��  [cutOff,outerCutOff] �ڵ������ǿ�𽥼���  ����outerCutOffΪ0

    //��Դ˥��
    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos; // �۾�λ��
uniform vec3 objectColor;  //������ɫ

void main() 
{
    //���嵽��Դ������
    vec3 lightDir = normalize(light.position - outFragPos);
    //��ͼ
    vec3 diffuseTexture = vec3(texture(material.diffuse, outTexCoord));//��������ͼ
    vec3 specularTexture = vec3(texture(material.specular, outTexCoord));//��������ͼ

    //����  //˥��ֵ
    float distance = length(light.position - outFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
   
    //�۹�Դ
    float theta = dot(lightDir, normalize(-light.direction)); //�������Դ�ļн�
    float epsilon = light.cutOff - light.outerCutOff;  
    //�н�theta��С��cutOff ���ǿΪ1   ����outerCutOff��Ϊ0   ������֮�䣬��ǿΪ[0��1]
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // ������
    vec3 ambient = light.ambient * diffuseTexture; 
    ambient  *= attenuation; 
    // ������
    vec3 norm = normalize(outNormal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseTexture; 
    // ���淴��
    vec3 viewDir = normalize(viewPos - outFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * specularTexture; 

    ambient  *= attenuation; 
    diffuse  *= attenuation * intensity;
    specular *= attenuation* intensity;
    vec3 result = (ambient + diffuse + specular) * vec3(objectColor);

    FragColor = vec4(result, 1.0);
}