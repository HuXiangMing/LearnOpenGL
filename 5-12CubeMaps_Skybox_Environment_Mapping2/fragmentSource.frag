#version 330 core
out vec4 FragColor;

in vec3 Normal;//���㷨��
in vec3 Position;//����λ��

uniform vec3 cameraPos;//�۲췽������
uniform samplerCube skybox;//��պ�����

void main()
{    

    //������������
    //����/���ߴӿ������벣����������Ǽ��������ǲ����Ƶģ������Ա�ֵΪ1.00/1.52=0.658
    float ratio = 1.00 / 1.52;//������
    vec3 I = normalize(Position - cameraPos);//�ӽǿ���ƬԪ�ķ���
    //�������ʹ��GLSL���ڽ�refract����������ʵ�֣�
    //����Ҫһ����������һ���۲췽�����������֮���������(Refractive Index)��
    vec3 R = refract(I,normalize(Normal),ratio); // ʹ��refract����������������
        
    FragColor = vec4(texture(skybox,R).rgb,1.0);

}