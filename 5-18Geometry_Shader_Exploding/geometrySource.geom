#version 330 core

layout (triangles) in;//�����ͼԪ����
layout (triangle_strip, max_vertices = 3) out;//�����ͼԪ���ͺ���󶥵���


//GLSL�ṩ������һ���ڽ�(Built-in)�������ڲ������������ܣ����£�
//in gl_Vertex
//{
//    vec4  gl_Position;
//    float gl_PointSize;
//    float gl_ClipDistance[];
//} gl_in[];

in VS_OUT {
    vec2 texCoords;
} gs_in[];

out vec2 TexCoords; 

uniform float time;

//����һ���µ�����������λ���������ŷ�����������λ��֮��Ľ��
vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    //sin��������һ��time������������ʱ�䷵��һ��-1.0��1.0֮���ֵ��
    //��Ϊ���ǲ������������ڱ�ը�����ǽ�sinֵ�任����[0, 1]�ķ�Χ��
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);
}

//���㵱ǰ��������������ķ���
vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main()
{   
//    int n;
//    //������������Ķ���
//    for(n=0; n<gl_in.length();n++)
//    {
//        gl_Position = gl_in[n].gl_Position;
//        TexCoords = gs_in[n].texCoords;
//        EmitVertex();
//    }

    //����������ķ�����
    vec3 normal = GetNormal();

    //�������γ�һ�������Σ�������Ƭ���ڵ���������һ���ƶ�
    gl_Position = explode(gl_in[0].gl_Position, normal);
    TexCoords = gs_in[0].texCoords;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    TexCoords = gs_in[1].texCoords;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    TexCoords = gs_in[2].texCoords;
    
    EmitVertex();
    EndPrimitive();
}