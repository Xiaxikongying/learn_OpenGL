#version 330 core
out float FragColor;

in vec2 TexCoords;
uniform sampler2D gPosition; //位置纹理
uniform sampler2D gNormal;  //法线纹理
uniform sampler2D texNoise; //生成的噪声纹理

uniform vec3 samples[64];//随机采样点

int kernelSize = 64; //采样次数
float radius = 0.5; //采样半径
float bias = 0.025; //深度偏移值

// 根据屏幕尺寸除以噪声大小在屏幕上平铺纹理  除以4是因为noise只有16个点
const vec2 noiseScale = vec2(1200.0 / 4.0, 900.0 / 4.0);

uniform mat4 projection;

void main()
{
  // 获取SSAO算法的输入
  vec3 fragPos = texture(gPosition, TexCoords).xyz;
  vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
  vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

  // 创建TBN矩阵，从切线空间到视图空间
  vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal)); //对切线做随机偏移
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN = mat3(tangent, bitangent, normal);

  // 计算遮挡因子
  float occlusion = 0.0;
  for(int i = 0; i < kernelSize; ++i )
  {
    // 获取样本位置
    vec3 samplePos = TBN * samples[i]; // 切线 -> 观察 space
    samplePos = fragPos + samplePos * radius;

    // 投影样本位置并且采样纹理，获取纹理上的位置
    vec4 offset = vec4(samplePos, 1.0);
    offset = projection * offset; // 观察 -> 裁剪 space
    offset.xyz /= offset.w; //[-1,1] 透视划分
    offset.xyz = offset.xyz * 0.5 + 0.5; // 变换到 [0,1]范围
    // 获取样本深度
    float sampleDepth = texture(gPosition, offset.xy).z; // 内核样本的深度值

    // 半径与深度差值的比   只有当深度值在取样半径内时才会影响遮挡因子  
    float rangCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth)); // 光滑插值第三个参数，在范围0.1到1.0之间
    occlusion += (sampleDepth >= (samplePos.z + bias) ? 1.0 : 0.0) * rangCheck;
  }
  //环境光的大小与  遮蔽因子数乘反比
  occlusion = 1.0 - (occlusion / kernelSize);
  FragColor = occlusion;
}