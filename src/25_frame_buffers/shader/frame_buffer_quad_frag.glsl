#version 330 core
out vec4 FragColor;

in vec2 outTexCoord;
uniform sampler2D screenTexture;

void main()
{ 
    // 1.正常显示
    //FragColor = vec4(texture(screenTexture, outTexCoord).rgb,1.0);//正常显示

    // 2.反相
    //FragColor= vec4(vec3(1.0 - texture(screenTexture, outTexCoord)),1.0); 

    // 3.灰度
    vec3 texColor = texture(screenTexture, outTexCoord).rgb;
    float average = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;
    FragColor = vec4(vec3(average), 1.0);


    // // 3.核效果
    // const float offset = 1.0 / 300.0;  
    // vec2 offsets[9] = vec2[](
    //     vec2(-offset,  offset), // 左上
    //     vec2( 0.0f,    offset), // 正上
    //     vec2( offset,  offset), // 右上
    //     vec2(-offset,  0.0f),   // 左
    //     vec2( 0.0f,    0.0f),   // 中
    //     vec2( offset,  0.0f),   // 右
    //     vec2(-offset, -offset), // 左下
    //     vec2( 0.0f,   -offset), // 正下
    //     vec2( offset, -offset)  // 右下
    // );
    // //3.1锐化核
    // // float kernel[9] = float[](
    // //     -1, -1, -1,
    // //     -1,  9, -1,
    // //     -1, -1, -1
    // // );

    // //3.2模糊核
    // // float kernel[9] = float[](
    // // 1.0 / 16, 2.0 / 16, 1.0 / 16,
    // // 2.0 / 16, 4.0 / 16, 2.0 / 16,
    // // 1.0 / 16, 2.0 / 16, 1.0 / 16  
    // // );



    // //3.3边缘检测核
    // // float kernel[9] = float[](
    // //     1, 1, 1,
    // //     1, -8, 1,
    // //     1, 1, 1
    // // );

    // vec3 sampleTex[9];
    // for(int i = 0; i < 9; i++)
    // {
    //     sampleTex[i] = vec3(texture(screenTexture, outTexCoord.st + offsets[i]));
    // }
    // vec3 col = vec3(0.0);
    // for(int i = 0; i < 9; i++)
    //     col += sampleTex[i] * kernel[i];

    // FragColor = vec4(col, 1.0);

}