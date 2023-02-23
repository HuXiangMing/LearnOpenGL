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
    // 当垂直看一个表面的时候纹理时位移比以一定角度看时的小。 
    // 我们可以在垂直看时使用更少的样本，以一定角度看时增加样本数量
    const float minLayers = 16;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // 深度层数
    //const float numLayers = 20;
    // 计算每一层的深度
    float layerDepth = 1.0 / numLayers;
    // 当前层深度
    float currentLayerDepth = 0.0;
    // 每一层沿着P¯的方向进行移动。
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;

    // 获取初始值
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;
    
    while(currentLayerDepth < currentDepthMapValue)
    {
        // 沿着P方向移动纹理坐标
        currentTexCoords -= deltaTexCoords;
        // 获取当前纹理坐标处的深度贴图值
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;  
        // 获取下一层的深度
        currentLayerDepth += layerDepth;  
    }
    
    // 在碰撞前获取纹理坐标(反向操作)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;//在对（位移的）表面几何进行交叉，找到深度层之后，我们获取交叉前的纹理坐标。

    // 得到线性插值碰撞前后的深度
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // 纹理坐标插值
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
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
