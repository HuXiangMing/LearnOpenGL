#version 330 core                                   
out vec4 FragColor; 

in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform vec3 objectColor;//物体反射的颜色
uniform vec3 lightColor;//灯光颜色

//uniform vec3 lightPos;//灯光的位置
//uniform vec3 viewPos;//摄像机的位置 不需要，观察坐标中摄像机的位置始终在（0，0）

void main(){    

    //环境光照
    float ambientStrength = 0.1f;//环境光照强度
    vec3 ambient = ambientStrength * lightColor;//环境光

    //漫反射光照
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);//片元->灯光
    float diff = max(dot(norm, lightDir), 0.0);//漫反射量
    vec3 diffuse = diff * lightColor;

    //镜面光照
    float specularStrength = 0.5;
    vec3 viewDir = normalize(-FragPos);//片元->摄像机
    vec3 reflectDir = reflect(-lightDir, norm);//reflect函数要求第一个向量是从光源指向片段位置的向量，返回一个根据法线对称的一个反射向量
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);//镜面光照：32是高光的反光度(Shininess)。一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小。
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}