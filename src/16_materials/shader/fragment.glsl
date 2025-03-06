#version 330 core
out vec4 FragColor;

in vec3 outFragPos;  //物体位置
in vec3 outNormal;   //法向量

// 定义材质结构体
struct Material
{
    vec3 ambient; // 物体对环境光的反射
    vec3 diffuse; // 物体对漫反射光的反射
    vec3 specular; // 物体对高光颜色的反射
    float shininess; // 物体的高光指数
};
// 光源属性
struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 objectColor;  //物体颜色
uniform vec3 viewPos;  //观测位置

void main()
{
    //环境光
    vec3 ambient = light.ambient * material.ambient;

    //漫反射光：diffuse
    vec3 norm = normalize(outNormal);
    vec3 lightDir = normalize(light.position - outFragPos); //光源方向
    float diff = max(dot(norm, lightDir), 0.0);     //入射光 与 法线 的夹角
    vec3 diffuse = light.diffuse * diff * material.diffuse; // 漫反射

    //镜面反射光：specular
    vec3 viewDir = normalize(viewPos - outFragPos); //观测方向
    vec3 reflectDir = reflect(-lightDir, norm);     //反射方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);  //反射光方向 与 观测方向 的夹角  
    vec3 specular = light.specular * spec * material.specular;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}