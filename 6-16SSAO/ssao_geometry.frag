#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

void main()
{    
    // ��Ƭ��λ�������洢�ڵ�һ��gbuffer������
    gPosition = FragPos;
    // ͬʱ��ÿ��Ƭ�εķ��ߴ洢��gbuffer��
    gNormal = normalize(Normal);
    // �Լ�Ĭ�ϵĵ�Ƭ����ɫ
    gAlbedo.rgb = vec3(0.95);
}