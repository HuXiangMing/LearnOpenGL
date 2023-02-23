#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

// 相关参数
int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

// 屏幕的平铺噪声纹理会根据屏幕分辨率除以噪声大小的值来决定
const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0); 

uniform mat4 projection;

void main()
{
    // 得到SSAO算法的输入
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
    // 创建TBN基变换矩阵:从切线空间到视图空间
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // 遍历样本内核并计算遮挡因子
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // 获取样品位置
        vec3 samplePos = TBN * samples[i]; // 从切线到视图空间
        samplePos = fragPos + samplePos * radius; 
        
        // 投影样本位置(采样纹理)(获得屏幕/纹理上的位置)
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // // 观察->裁剪空间 from view to clip-space
        offset.xyz /= offset.w; // 透视划分
        offset.xyz = offset.xyz * 0.5 + 0.5; // 变换到0.0 - 1.0的值域
        
        // 获取样本深度
        float sampleDepth = texture(gPosition, offset.xy).z; // 得到内核样本的深度值
        
        // 范围检查和累积
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
}
