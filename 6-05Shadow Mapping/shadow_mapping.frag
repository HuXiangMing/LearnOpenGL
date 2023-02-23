#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // ִ��͸�ӳ���->Ƭ���ڹ�ռ��-1��1�ķ�Χ��
    //���ڲ��пռ��FragPosLightSpace������ͨ��gl_Position����Ƭ����ɫ������Ǳ����Լ���͸�ӳ���
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // �任��[0,1]�ķ�Χ->Ϊ�˺������ͼ�������Ƚϣ�z������Ҫ�任��[0,1],Ϊ����Ϊ�������ͼ�в��������꣬xy����Ҳ��Ҫ�任��[0,1]
    // ��Ϊ���������ͼ�������0��1�ķ�Χ������Ҳ����ʹ��projCoords�������ͼ��ȥ�������������ǽ�NDC����任Ϊ0��1�ķ�Χ��
    projCoords = projCoords * 0.5 + 0.5;
    // ȡ�����������(ʹ��[0,1]��Χ�µ�fragPosLight������)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // ȡ�õ�ǰƬ���ڹ�Դ�ӽ��µ����
    float currentDepth = projCoords.z;
    // ��鵱ǰƬ���Ƿ�����Ӱ��
    //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    //��Ӱƫ��
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    //һ��ƫ�ƾ��ܰﵽ�ܴ��æ��������Щ�����¶Ⱥܴ���Ȼ�������Ӱʧ�档ʹ�õ�˻�����ʣ�������ʧ������
    //���ֵ0.05����һ����Сֵ0.005�������ǻ��ڱ��淨�ߺ͹��շ���ġ�
    //������ذ������ı��漸�����Դ��ֱ���õ���ƫ�ƾͺ�С
    //������������Ĳ������ֱ���õ���ƫ�ƾ͸���
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);   
    //shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    //PCF_������Ӱ��ݿ�
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    // �ڹ��߽�׶��far_plane�����Ᵽ����ӰΪ0.0����Ϊ�����ڹ�Դ��׶�ļ�Զ�������ǻ����1.0
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

void main()
{          
    // ʹ��Blinn-Phong����ģ����Ⱦ����
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
    // Ambient
    vec3 ambient = 0.15 * color;
    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    

    // ������Ӱ
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    

    FragColor = vec4(lighting, 1.0f);
}