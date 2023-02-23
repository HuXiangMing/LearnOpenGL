#version 330 core

layout (points) in;//输入的图元类型
layout (triangle_strip, max_vertices = 5) out;//输出的图元类型和最大顶点数


//GLSL提供给我们一个内建(Built-in)变量，内部看起来（可能）如下：
//in gl_Vertex
//{
//    vec4  gl_Position;
//    float gl_PointSize;
//    float gl_ClipDistance[];
//} gl_in[];

//因为几何着色器是作用于输入的一组顶点的，从顶点着色器发来输入数据总是会以数组的形式表示出来
//即便我们现在只有一个顶点。
//图元类型                 输入数组的大小
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
    fColor = gs_in[0].color;//gs_in[0] 因为只有一个输入顶点
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:左下
    EmitVertex();   
    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:右下
    EmitVertex();
    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:左上
    EmitVertex();
    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:右上
    EmitVertex();
    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:顶部
    //将最后一个顶点的颜色设置为白色
    fColor = vec3(1.0, 1.0, 1.0);
    EmitVertex();
    EndPrimitive();
}
void main()
{   
    build_house(gl_in[0].gl_Position);
}