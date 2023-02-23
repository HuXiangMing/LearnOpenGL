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
uniform sampler2D normalMap;//法线贴图
uniform sampler2D depthMap;//深度（高度）贴图

uniform float heightScale;

//uniform vec3 lightPos;
//uniform vec3 viewPos;

//把fragment的纹理坐标和切线空间中的fragment到观察者的方向向量为输入。
//这个函数返回经位移的纹理坐标。然后我们使用这些经位移的纹理坐标进行diffuse和法线贴图的采样。
//最后fragment的diffuse颜色和法线向量就正确的对应于表面的经位移的位置上了。
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height =  texture(depthMap, texCoords).r;  //得到当前片元的高度  
    return texCoords - viewDir.xy * (height * heightScale); //用片元的高度对它进行缩放，相减来获得最终的经过位移纹理坐标     
}

void main()
{     

    // 偏移纹理坐标视差映射
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = fs_in.TexCoords;
    
    //纹理坐标超出了0到1的范围进行采样，根据纹理的环绕方式导致了不真实的结果。所以丢弃超出默认纹理坐标范围的纹理
    texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);       
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;


    // 从法线映射中获取法线
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
