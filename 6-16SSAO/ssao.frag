#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

// ��ز���
int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

// ��Ļ��ƽ����������������Ļ�ֱ��ʳ���������С��ֵ������
const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0); 

uniform mat4 projection;

void main()
{
    // �õ�SSAO�㷨������
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
    // ����TBN���任����:�����߿ռ䵽��ͼ�ռ�
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // ���������ں˲������ڵ�����
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // ��ȡ��Ʒλ��
        vec3 samplePos = TBN * samples[i]; // �����ߵ���ͼ�ռ�
        samplePos = fragPos + samplePos * radius; 
        
        // ͶӰ����λ��(��������)(�����Ļ/�����ϵ�λ��)
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // // �۲�->�ü��ռ� from view to clip-space
        offset.xyz /= offset.w; // ͸�ӻ���
        offset.xyz = offset.xyz * 0.5 + 0.5; // �任��0.0 - 1.0��ֵ��
        
        // ��ȡ�������
        float sampleDepth = texture(gPosition, offset.xy).z; // �õ��ں����������ֵ
        
        // ��Χ�����ۻ�
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
}
