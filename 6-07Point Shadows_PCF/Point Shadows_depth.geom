#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;//输入一个三角形，输出总共6个三角形（6*3顶点，所以总共18个顶点）


uniform mat4 shadowMatrices[6];//光空间变换矩阵

out vec4 FragPos; // FragPos从GS(输出每个发射顶点)

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        //几何着色器有一个内建变量叫做gl_Layer，它指定发散出基本图形送到立方体贴图的哪个面。
        //当不管它时，几何着色器就会像往常一样把它的基本图形发送到输送管道的下一阶段，但当我们更新这个变量就能控制每个基本图形将渲染到立方体贴图的哪一个面。
        gl_Layer = face; 
        for(int i = 0; i < 3; ++i) // 对于每个三角形的顶点
        {
            //把面的光空间变换矩阵乘以FragPos，将每个世界空间顶点变换到相关的光空间，生成每个三角形。
            FragPos = gl_in[i].gl_Position;//将最后的FragPos变量发送给像素着色器，我们需要计算一个深度值。
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}