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
    float cutOff;//cosValue切光角（0->切光交以内的片元都能够被看见） cosValue
    float outerCutOff;//cosValue外围角（切光交->外围角的片元会做平滑处理）

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //衰减系数
    float constant;
    float linear;
    float quadratic;
};


in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;//摄像机的位置
uniform Material material;
uniform Light light;

void main(){    

    //环境光照
    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));//环境光

    //漫反射光照
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);//漫反射量
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse,TexCoords)));

    //镜面光照
    vec3 viewDir = normalize(viewPos - FragPos);//片元->摄像机
    vec3 reflectDir = reflect(-lightDir, norm);//reflect函数要求第一个向量是从光源指向片段位置的向量，返回一个根据法线对称的一个反射向量
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);//镜面光照：32是高光的反光度(Shininess)。一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小。
    vec3 specular = light.specular * (spec * vec3(texture(material.specular,TexCoords)));

    //聚光灯(平滑)
    float theta = dot(lightDir, normalize(-light.direction));
    if(theta > light.cutOff) //cos值越大，角度越小
    {       
        // 执行正常光照计算
    }
    else  // 否则，使用环境光，让场景在聚光之外时不至于完全黑暗
    {
        float epsilon = (light.cutOff - light.outerCutOff);
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);//线性过度（限制在0-1之间）
        diffuse  *= intensity;
        specular *= intensity;
    }

    //衰减
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation;  

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}