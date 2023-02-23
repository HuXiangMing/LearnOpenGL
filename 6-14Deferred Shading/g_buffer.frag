#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{    
    // ��Ƭ��λ�������洢�ڵ�һ��gbuffer������
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // ͬʱ��ÿ��Ƭ�εķ��ߴ洢��gbuffer��
    gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    // ��gAlbedoSpec��alpha����д洢�߹�ǿ��
    gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
}