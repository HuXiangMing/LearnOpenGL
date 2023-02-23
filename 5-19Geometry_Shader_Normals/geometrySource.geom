#version 330 core

layout (triangles) in;//输入的图元类型
layout (line_strip, max_vertices = 6) out;//输出的图元类型和最大顶点数


//GLSL提供给我们一个内建(Built-in)变量，内部看起来（可能）如下：
//in gl_Vertex
//{
//    vec4  gl_Position;
//    float gl_PointSize;
//    float gl_ClipDistance[];
//} gl_in[];

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.4;

uniform mat4 projection;

//生成线
void GenerateLine(int index)
{
     //两个点形成一条线
     gl_Position = projection * gl_in[index].gl_Position;
     EmitVertex();
     gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);

     EmitVertex();
     EndPrimitive();
}

void main()
{   

    GenerateLine(0); // 第一个顶点法线
    GenerateLine(1); // 第二个顶点法线
    GenerateLine(2); // 第三个顶点法线
}