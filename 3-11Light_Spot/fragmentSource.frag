#version 330 core                                   
out vec4 FragColor; 

struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct Light {
    vec3  position;
    vec3  direction;
    float cutOff;//cosValue�й�ǣ�0->�й⽻���ڵ�ƬԪ���ܹ��������� cosValue
    float outerCutOff;//cosValue��Χ�ǣ��й⽻->��Χ�ǵ�ƬԪ����ƽ������

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //˥��ϵ��
    float constant;
    float linear;
    float quadratic;
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
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);//��������
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse,TexCoords)));

    //�������
    vec3 viewDir = normalize(viewPos - FragPos);//ƬԪ->�����
    vec3 reflectDir = reflect(-lightDir, norm);//reflect����Ҫ���һ�������Ǵӹ�Դָ��Ƭ��λ�õ�����������һ�����ݷ��߶ԳƵ�һ����������
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);//������գ�32�Ǹ߹�ķ����(Shininess)��һ������ķ����Խ�ߣ�����������Խǿ��ɢ���Խ�٣��߹��ͻ�ԽС��
    vec3 specular = light.specular * (spec * vec3(texture(material.specular,TexCoords)));

    //�۹��(ƽ��)
    float theta = dot(lightDir, normalize(-light.direction));
    if(theta > light.cutOff) //cosֵԽ�󣬽Ƕ�ԽС
    {       
        // ִ���������ռ���
    }
    else  // ����ʹ�û����⣬�ó����ھ۹�֮��ʱ��������ȫ�ڰ�
    {
        float epsilon = (light.cutOff - light.outerCutOff);
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);//���Թ��ȣ�������0-1֮�䣩
        diffuse  *= intensity;
        specular *= intensity;
    }

    //˥��
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation;  

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}