#version 330 core                                   
out vec4 FragColor; 

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;//摄像机的位置
uniform Material material;
uniform Light light;

void main(){    

    //环境光照
    vec3 ambient = light.ambient * material.ambient;//环境光

    //漫反射光照
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);//片元->灯光
    float diff = max(dot(norm, lightDir), 0.0);//漫反射量
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    //镜面光照
    vec3 viewDir = normalize(viewPos - FragPos);//片元->摄像机
    vec3 reflectDir = reflect(-lightDir, norm);//reflect函数要求第一个向量是从光源指向片段位置的向量，返回一个根据法线对称的一个反射向量
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);//镜面光照：32是高光的反光度(Shininess)。一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小。
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}