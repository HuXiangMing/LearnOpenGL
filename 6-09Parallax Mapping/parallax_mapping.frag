#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;//������ͼ
uniform sampler2D depthMap;//��ȣ��߶ȣ���ͼ

uniform float heightScale;

//uniform vec3 lightPos;
//uniform vec3 viewPos;

//��fragment��������������߿ռ��е�fragment���۲��ߵķ�������Ϊ���롣
//����������ؾ�λ�Ƶ��������ꡣȻ������ʹ����Щ��λ�Ƶ������������diffuse�ͷ�����ͼ�Ĳ�����
//���fragment��diffuse��ɫ�ͷ�����������ȷ�Ķ�Ӧ�ڱ���ľ�λ�Ƶ�λ�����ˡ�
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height =  texture(depthMap, texCoords).r;  //�õ���ǰƬԪ�ĸ߶�  
    return texCoords - viewDir.xy * (height * heightScale); //��ƬԪ�ĸ߶ȶ����������ţ������������յľ���λ����������     
}

void main()
{     

    // ƫ�����������Ӳ�ӳ��
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = fs_in.TexCoords;
    
    //�������곬����0��1�ķ�Χ���в�������������Ļ��Ʒ�ʽ�����˲���ʵ�Ľ�������Զ�������Ĭ���������귶Χ������
    texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);       
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;


    // �ӷ���ӳ���л�ȡ����
    vec3 normal = texture(normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
   
    // get diffuse color
    vec3 color = texture(diffuseMap, texCoords).rgb;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular    
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
