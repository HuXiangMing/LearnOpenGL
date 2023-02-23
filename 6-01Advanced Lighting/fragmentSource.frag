#version 330 core
out vec4 FragColor;
in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
}fs_in;

uniform sampler2D floorTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool blinn;

void main()
{          
    vec3 color = texture(floorTexture, fs_in.TexCoords).rgb;

    //��������
    float ambientStrength = 0.05f;//��������ǿ��
    vec3 ambient = ambientStrength * color;//������

    //��������� Phong ����ģ�ͼ�����߱����ȵķ��䵽���������ǿ�ȶ���ͬ����������ʲô����۲���������ȶ�һ����
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);//ƬԪ->�ƹ�
    float diff = max(dot(norm,lightDir), 0.0);//��������
    vec3 diffuse = diff * color;

    //�������
    float specularStrength = 1.0;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);//ƬԪ->�����    
    float spec = 0.0f;//���������
    if(blinn)
    {
        //Blinn-Phong
        //�������(Halfway Vector)
        vec3 halfwayDir = normalize(lightDir + viewDir);  //��ȡ��������ķ����ܼ򵥣�ֻ��Ҫ�����ߵķ��������͹۲������ӵ�һ�𼴿�
        spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    }
    else
    {
        //���Ϲ���
        //��������
        vec3 reflectDir = reflect(-lightDir, norm);//reflect����Ҫ���һ�������Ǵӹ�Դָ��Ƭ��λ�õ�����������һ�����ݷ��߶ԳƵ�һ����������
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }  
    vec3 specular = specularStrength * spec *vec3(0.3);
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}