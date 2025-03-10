#version 330 core
out vec4 FragColor;

in vec2 oTexCoord;
in vec3 oNormal;
in vec3 oPosition;

uniform vec3 cameraPos;
uniform samplerCube cubeTexture; //天空盒纹理
uniform vec3 objectColor;

void main()
{             
    float ratio = 1.00 / 1.52; //折射率
    vec3 I = normalize(oPosition - cameraPos);  //观察向量
    vec3 R = refract(I, normalize(oNormal), ratio);//反射向量
    FragColor = vec4(texture(cubeTexture, R).rgb + objectColor, 1.0);
}