#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;


//���Դ��Ӱ������ƽ�й���Ӱ������Щ����
float ShadowCalculation(vec3 fragPos)
{
    //��ȡ��������ͼ����ȣ���Ϊ�����Ѿ�����ȴ���Ϊfragment�͹�λ��֮��ľ�����
    vec3 fragToLight = fragPos - lightPos; // ���������Ϊһ����������ȥ����������ͼ���в�����������������Ҫ�ǵ�λ��������������������б�׼����
    float closestDepth = texture(depthMap, fragToLight).r;//��Դ������ӽ��Ŀɼ�fragment֮��ı�׼�������ֵ��
    
    //closestDepthֵ������0��1�ķ�Χ���ˣ����������Ƚ���ת����0��far_plane�ķ�Χ
    closestDepth *= far_plane;
    //��ȡ��ǰfragment�͹�Դ֮������ֵ�����ǿ��Լ򵥵�ʹ��fragToLight�ĳ�������ȡ��
    float currentDepth = length(fragToLight);
    // ����Ƿ�����Ӱ������
    float bias = 0.05; // ����ʹ���˸����ƫ���Ϊ���������[near_plane, far_plane]��Χ��
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    // ��closestDepth��ʾΪ����(�Կ��ӻ����������ͼ)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
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
    float shadow = shadows ? ShadowCalculation(fs_in.FragPos) : 0.0;        
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    

    FragColor = vec4(lighting, 1.0f);
}