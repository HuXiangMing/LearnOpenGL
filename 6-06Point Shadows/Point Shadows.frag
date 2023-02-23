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


//点光源阴影计算与平行光阴影计算有些差异
float ShadowCalculation(vec3 fragPos)
{
    //获取立方体贴图的深度，因为我们已经将深度储存为fragment和光位置之间的距离了
    vec3 fragToLight = fragPos - lightPos; // 这个向量作为一个方向向量去对立方体贴图进行采样。方向向量不需要是单位向量，所以无需对它进行标准化。
    float closestDepth = texture(depthMap, fragToLight).r;//光源和它最接近的可见fragment之间的标准化的深度值。
    
    //closestDepth值现在在0到1的范围内了，所以我们先将其转换回0到far_plane的范围
    closestDepth *= far_plane;
    //获取当前fragment和光源之间的深度值，我们可以简单的使用fragToLight的长度来获取它
    float currentDepth = length(fragToLight);
    // 检查是否在阴影区域内
    float bias = 0.05; // 我们使用了更大的偏差，因为深度现在在[near_plane, far_plane]范围内
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    // 将closestDepth显示为调试(以可视化深度立方体图)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

void main()
{          
    // 使用Blinn-Phong光照模型渲染场景
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

    // 计算阴影
    float shadow = shadows ? ShadowCalculation(fs_in.FragPos) : 0.0;        
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    

    FragColor = vec4(lighting, 1.0f);
}