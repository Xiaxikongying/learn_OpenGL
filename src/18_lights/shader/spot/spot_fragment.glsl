#version 330 core
out vec4 FragColor;
in vec2 outTexCoord;
in vec3 outNormal;
in vec3 outFragPos;

// 定义材质结构体
struct Material 
{
    sampler2D diffuse; // 漫反射贴图
    sampler2D specular; // 镜面光贴图
    float shininess; // 高光指数
};

// 光源属性
struct Light 
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3  direction; //光照方向
    float cutOff;    //切光角   在此范围内 光强为1
    float outerCutOff; // 外切光角  [cutOff,outerCutOff] 内的物体光强逐渐减少  大于outerCutOff为0

    //光源衰减
    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos; // 眼睛位置
uniform vec3 objectColor;  //物体颜色

void main() 
{
    //物体到光源的向量
    vec3 lightDir = normalize(light.position - outFragPos);
    //贴图
    vec3 diffuseTexture = vec3(texture(material.diffuse, outTexCoord));//漫反射贴图
    vec3 specularTexture = vec3(texture(material.specular, outTexCoord));//环境光贴图

    //距离  //衰减值
    float distance = length(light.position - outFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
   
    //聚光源
    float theta = dot(lightDir, normalize(-light.direction)); //物体与光源的夹角
    float epsilon = light.cutOff - light.outerCutOff;  
    //夹角theta若小于cutOff 则光强为1   大于outerCutOff则为0   处于者之间，光强为[0，1]
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // 环境光
    vec3 ambient = light.ambient * diffuseTexture; 
    ambient  *= attenuation; 
    // 漫反射
    vec3 norm = normalize(outNormal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseTexture; 
    // 镜面反射
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