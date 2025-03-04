#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord; //纹理坐标

uniform sampler2D Texture1; //纹理图像id  sampler2d是采样器，本质就是整数
uniform sampler2D Texture2;

void main()
{
    //FragColor = texture(Texture1, TexCoord);  //texture  从纹理图像中取出对应坐标的颜色
    //FragColor = texture(Texture1, TexCoord) * vec4(ourColor, 1.0);
    FragColor = mix(texture(Texture1, TexCoord), texture(Texture2, TexCoord), 0.2); //线性插值两个纹理 
    //第三个参数：1表示返回第一个纹理  0表示返回第二个纹理
}