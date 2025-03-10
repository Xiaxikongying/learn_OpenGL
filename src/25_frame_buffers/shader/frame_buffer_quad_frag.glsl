#version 330 core
out vec4 FragColor;

in vec2 outTexCoord;
uniform sampler2D screenTexture;

void main()
{ 
    // 1.������ʾ
    //FragColor = vec4(texture(screenTexture, outTexCoord).rgb,1.0);//������ʾ

    // 2.����
    //FragColor= vec4(vec3(1.0 - texture(screenTexture, outTexCoord)),1.0); 

    // 3.�Ҷ�
    vec3 texColor = texture(screenTexture, outTexCoord).rgb;
    float average = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;
    FragColor = vec4(vec3(average), 1.0);


    // // 3.��Ч��
    // const float offset = 1.0 / 300.0;  
    // vec2 offsets[9] = vec2[](
    //     vec2(-offset,  offset), // ����
    //     vec2( 0.0f,    offset), // ����
    //     vec2( offset,  offset), // ����
    //     vec2(-offset,  0.0f),   // ��
    //     vec2( 0.0f,    0.0f),   // ��
    //     vec2( offset,  0.0f),   // ��
    //     vec2(-offset, -offset), // ����
    //     vec2( 0.0f,   -offset), // ����
    //     vec2( offset, -offset)  // ����
    // );
    // //3.1�񻯺�
    // // float kernel[9] = float[](
    // //     -1, -1, -1,
    // //     -1,  9, -1,
    // //     -1, -1, -1
    // // );

    // //3.2ģ����
    // // float kernel[9] = float[](
    // // 1.0 / 16, 2.0 / 16, 1.0 / 16,
    // // 2.0 / 16, 4.0 / 16, 2.0 / 16,
    // // 1.0 / 16, 2.0 / 16, 1.0 / 16  
    // // );



    // //3.3��Ե����
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