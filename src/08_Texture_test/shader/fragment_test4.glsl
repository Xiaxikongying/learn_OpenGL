#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord; //纹理坐标

uniform sampler2D Texture1; //纹理图像id  sampler2d是采样器，本质就是整数
uniform sampler2D Texture2;
uniform float mixValue;

void main()
{
    FragColor = mix(texture(Texture1, TexCoord), texture(Texture2, TexCoord), mixValue); //线性插值两个纹理 
}