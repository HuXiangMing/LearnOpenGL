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

    //环境光照
    float ambientStrength = 0.05f;//环境光照强度
    vec3 ambient = ambientStrength * color;//环境光

    //漫反射光照 Phong 反射模型假设光线被均匀的反射到各个方向的强度都相同，这样无论什么方向观察物体的亮度都一样。
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);//片元->灯光
    float diff = max(dot(norm,lightDir), 0.0);//漫反射量
    vec3 diffuse = diff * color;

    //镜面光照
    float specularStrength = 1.0;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);//片元->摄像机    
    float spec = 0.0f;//镜面光照量
    if(blinn)
    {
        //Blinn-Phong
        //半程向量(Halfway Vector)
        vec3 halfwayDir = normalize(lightDir + viewDir);  //获取半程向量的方法很简单，只需要将光线的方向向量和观察向量加到一起即可
        spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    }
    else
    {
        //冯氏光照
        //反射向量
        vec3 reflectDir = reflect(-lightDir, norm);//reflect函数要求第一个向量是从光源指向片段位置的向量，返回一个根据法线对称的一个反射向量
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }  
    vec3 specular = specularStrength * spec *vec3(0.3);
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}