#version 330 core                                   
out vec4 FragColor; 

in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;//物体反射的颜色
uniform vec3 lightColor;//灯光颜色
uniform vec3 lightPos;//灯光的位置

void main(){    

    //环境光照
    float ambientStrength = 0.1f;//环境光照强度
    vec3 ambient = ambientStrength * lightColor;//环境光

    //漫反射光照 Phong 反射模型假设光线被均匀的反射到各个方向的强度都相同，这样无论什么方向观察物体的亮度都一样。
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);//片元->灯光
    float diff = max(dot(norm, lightDir), 0.0);//漫反射量
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}