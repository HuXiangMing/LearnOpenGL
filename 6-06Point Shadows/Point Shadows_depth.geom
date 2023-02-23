#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;//����һ�������Σ�����ܹ�6�������Σ�6*3���㣬�����ܹ�18�����㣩


uniform mat4 shadowMatrices[6];//��ռ�任����

out vec4 FragPos; // FragPos��GS(���ÿ�����䶥��)

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        //������ɫ����һ���ڽ���������gl_Layer����ָ����ɢ������ͼ���͵���������ͼ���ĸ��档
        //��������ʱ��������ɫ���ͻ�������һ�������Ļ���ͼ�η��͵����͹ܵ�����һ�׶Σ��������Ǹ�������������ܿ���ÿ������ͼ�ν���Ⱦ����������ͼ����һ���档
        gl_Layer = face; 
        for(int i = 0; i < 3; ++i) // ����ÿ�������εĶ���
        {
            //����Ĺ�ռ�任�������FragPos����ÿ������ռ䶥��任����صĹ�ռ䣬����ÿ�������Ρ�
            FragPos = gl_in[i].gl_Position;//������FragPos�������͸�������ɫ����������Ҫ����һ�����ֵ��
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}