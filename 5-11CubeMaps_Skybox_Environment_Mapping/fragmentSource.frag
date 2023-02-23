#version 330 core
out vec4 FragColor;

in vec3 Normal;//���㷨��
in vec3 Position;//����λ��

uniform vec3 cameraPos;//�۲췽������
uniform samplerCube skybox;//��պ�����

void main()
{    

    //���㷴������

    vec3 I = normalize(Position - cameraPos);//�ӽǿ���ƬԪ�ķ���
    vec3 R = reflect(I,normalize(Normal)); // ʹ��reflect�������㷴������
        
    FragColor = vec4(texture(skybox,R).rgb,1.0);

}