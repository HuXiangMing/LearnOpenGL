#version 330 core

layout (points) in;//�����ͼԪ����
layout (triangle_strip, max_vertices = 5) out;//�����ͼԪ���ͺ���󶥵���


//GLSL�ṩ������һ���ڽ�(Built-in)�������ڲ������������ܣ����£�
//in gl_Vertex
//{
//    vec4  gl_Position;
//    float gl_PointSize;
//    float gl_ClipDistance[];
//} gl_in[];

//��Ϊ������ɫ���������������һ�鶥��ģ��Ӷ�����ɫ�����������������ǻ����������ʽ��ʾ����
//������������ֻ��һ�����㡣
//ͼԪ����                 ��������Ĵ�С
//points                    1
//lines                     2
//triangles                 3
//lines_adjacency           4
//triangles_adjacency       6
in VS_OUT {
    vec3 color;
} gs_in[];

out vec3 fColor;

void build_house(vec4 position)
{    
    fColor = gs_in[0].color;//gs_in[0] ��Ϊֻ��һ�����붥��
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:����
    EmitVertex();   
    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:����
    EmitVertex();
    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:����
    EmitVertex();
    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:����
    EmitVertex();
    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:����
    //�����һ���������ɫ����Ϊ��ɫ
    fColor = vec3(1.0, 1.0, 1.0);
    EmitVertex();
    EndPrimitive();
}
void main()
{   
    build_house(gl_in[0].gl_Position);
}