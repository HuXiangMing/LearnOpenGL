#version 330 core                                   
out vec4 FragColor; 

in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;//���巴�����ɫ
uniform vec3 lightColor;//�ƹ���ɫ
uniform vec3 lightPos;//�ƹ��λ��
uniform vec3 viewPos;//�������λ��

void main(){    

    //��������
    float ambientStrength = 0.1f;//��������ǿ��
    vec3 ambient = ambientStrength * lightColor;//������

    //���������
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);//ƬԪ->�ƹ�
    float diff = max(dot(norm, lightDir), 0.0);//��������
    vec3 diffuse = diff * lightColor;

    //�������
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);//ƬԪ->�����
    vec3 reflectDir = reflect(-lightDir, norm);//reflect����Ҫ���һ�������Ǵӹ�Դָ��Ƭ��λ�õ�����������һ�����ݷ��߶ԳƵ�һ����������
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);//������գ�32�Ǹ߹�ķ����(Shininess)��һ������ķ����Խ�ߣ�����������Խǿ��ɢ���Խ�٣��߹��ͻ�ԽС��
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}