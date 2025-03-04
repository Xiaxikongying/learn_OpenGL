#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord; //纹理坐标

uniform sampler2D Texture1; //纹理图像id  sampler2d是采样器，本质就是整数
uniform sampler2D Texture2;

void main()
{
    //1.0-TexCoord.x改变笑脸朝向
    FragColor = mix(texture(Texture1, TexCoord), texture(Texture2, vec2(1.0 - TexCoord.x, TexCoord.y)), 0.2); //线性插值两个纹理 
}