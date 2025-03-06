#version 330 core
out vec4 FragColor;

in vec3 outFragPos;  //物体位置
in vec3 outNormal;   //法向量

uniform vec3 objectColor;  //物体颜色
uniform vec3 lightColor;   //灯光颜色
uniform vec3 lightPos;  //光源位置
uniform vec3 viewPos;  //观测位置
uniform float ambientStrength; //环境光强度
uniform float diffuseStrength; //漫反射强度
uniform float specularStrength; //镜面反射强度 (即物体的光滑程度)


void main()
{
    //环境光
    vec3 ambient = ambientStrength * lightColor; 

    //漫反射光：diffuse
    vec3 norm = normalize(outNormal);
    vec3 lightDir = normalize(lightPos - outFragPos); //光源方向
    float diff = max(dot(norm, lightDir), 0.0);     //入射光 与 法线 的夹角
    vec3 diffuse = diff * lightColor;

    //镜面反射光：specular
    vec3 viewDir = normalize(viewPos - outFragPos); //观测方向
    vec3 reflectDir = reflect(-lightDir, norm);     //反射方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);  //反射光方向 与 观测方向 的夹角  32表示高光范围
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}