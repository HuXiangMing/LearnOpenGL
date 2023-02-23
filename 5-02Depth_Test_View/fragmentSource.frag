#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

//���ͶӰ����
float near = 0.1; 
float far  = 100.0; 


//����Ļ�ռ����ֵ�ķ����ԣ�תΪ����
float LinearizeDepth(float depth) 
{
    //���Ƚ����ֵ�任ΪNDC(�ӡ�0-1����Χ���±任����-1��1����Χ)
    float z = depth * 2.0 - 1.0;
    //��ȡ����zֵ��Ӧ����任����ȡ���Ե����ֵ��
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{    
    //gl_FragCoord������Zֵ�������Ǹ��ض�Ƭ�ε����ֵ
    //������ƬԪʱ��Zֵ������С����ʾΪ��ɫ
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // ���������ֵ����far�����ǽ��Ƶؽ��������ֵת����[0, 1]�ķ�Χ֮�䡣
    FragColor = vec4(vec3(depth), 1.0);
}