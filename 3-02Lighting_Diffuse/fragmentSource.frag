#version 330 core                                   
out vec4 FragColor; 

in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;//���巴�����ɫ
uniform vec3 lightColor;//�ƹ���ɫ
uniform vec3 lightPos;//�ƹ��λ��

void main(){    

    //��������
    float ambientStrength = 0.1f;//��������ǿ��
    vec3 ambient = ambientStrength * lightColor;//������

    //��������� Phong ����ģ�ͼ�����߱����ȵķ��䵽���������ǿ�ȶ���ͬ����������ʲô����۲���������ȶ�һ����
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);//ƬԪ->�ƹ�
    float diff = max(dot(norm, lightDir), 0.0);//��������
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}