#version 330 core
out vec4 FragColor;

in vec3 Normal;//顶点法线
in vec3 Position;//顶点位置

uniform vec3 cameraPos;//观察方向向量
uniform samplerCube skybox;//天空盒纹理

void main()
{    

    //计算反射向量

    vec3 I = normalize(Position - cameraPos);//视角看量片元的方向
    vec3 R = reflect(I,normalize(Normal)); // 使用reflect函数计算反射向量
        
    FragColor = vec4(texture(skybox,R).rgb,1.0);

}