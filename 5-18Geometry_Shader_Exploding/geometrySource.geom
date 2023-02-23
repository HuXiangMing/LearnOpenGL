#version 330 core

layout (triangles) in;//输入的图元类型
layout (triangle_strip, max_vertices = 3) out;//输出的图元类型和最大顶点数


//GLSL提供给我们一个内建(Built-in)变量，内部看起来（可能）如下：
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

//返回一个新的向量，它是位置向量沿着法线向量进行位移之后的结果
vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    //sin函数接收一个time参数，它根据时间返回一个-1.0到1.0之间的值。
    //因为我们不想让物体向内爆炸，我们将sin值变换到了[0, 1]的范围内
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);
}

//计算当前顶点所在三角面的法线
vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main()
{   
//    int n;
//    //遍历所有输入的顶点
//    for(n=0; n<gl_in.length();n++)
//    {
//        gl_Position = gl_in[n].gl_Position;
//        TexCoords = gs_in[n].texCoords;
//        EmitVertex();
//    }

    //顶点所在面的法向量
    vec3 normal = GetNormal();

    //三个点形成一个三角形，三角面片所在的三个顶点一起移动
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