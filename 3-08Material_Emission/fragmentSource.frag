#version 330 core                                   
out vec4 FragColor; 

struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;   
    sampler2D emission;
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;//�������λ��
uniform Material material;
uniform Light light;

void main(){    

    //��������
    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));//������

    //���������
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);//ƬԪ->�ƹ�
    float diff = max(dot(norm, lightDir), 0.0);//��������
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse,TexCoords)));

    //�������
    vec3 viewDir = normalize(viewPos - FragPos);//ƬԪ->�����
    vec3 reflectDir = reflect(-lightDir, norm);//reflect����Ҫ���һ�������Ǵӹ�Դָ��Ƭ��λ�õ�����������һ�����ݷ��߶ԳƵ�һ����������
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);//������գ�32�Ǹ߹�ķ����(Shininess)��һ������ķ����Խ�ߣ�����������Խǿ��ɢ���Խ�٣��߹��ͻ�ԽС��
    vec3 specular = light.specular * (spec * vec3(texture(material.specular,TexCoords)));

    //�Է���
    vec3 emission = texture(material.emission,TexCoords).rgb;

    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0);
}