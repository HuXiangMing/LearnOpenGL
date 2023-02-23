#version 330 core   
layout (location = 0) in vec3 aPos;  
layout (location = 1) in vec3 aNormal;//���㷨��

out vec3 LightingColor; // �ƹ���������֮�����ɫ�����ݸ�ƬԪ��ɫ��

uniform vec3 lightColor;//�ƹ���ɫ
uniform vec3 lightPos;//�ƹ��λ��
uniform vec3 viewPos;//�������λ��

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){  

	//ͨ���Ѷ���λ�����Գ���ģ�;��󣨲��ǹ۲��ͶӰ�����������任������ռ�����
	vec3  FragPos = vec3(model * vec4(aPos, 1.0));
	//Normal = aNormal;����������model������������ת�����ţ���ʱ�ķ���ȴ����ԭ�������ݣ�������������Ч��
	vec3  Normal = mat3(transpose(inverse(model))) * aNormal;	//ʹ��inverse��transpose����������߾���
	gl_Position = projection * view * vec4(FragPos, 1.0); 
	

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

    LightingColor = ambient + diffuse + specular;
	
}	