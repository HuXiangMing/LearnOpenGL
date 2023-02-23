#version 330 core
out vec4 FragColor;

in vec3 Normal;//顶点法线
in vec3 Position;//顶点位置

uniform vec3 cameraPos;//观察方向向量
uniform samplerCube skybox;//天空盒纹理

void main()
{    

    //计算折射向量
    //光线/视线从空气进入玻璃，如果我们假设箱子是玻璃制的），所以比值为1.00/1.52=0.658
    float ratio = 1.00 / 1.52;//折射率
    vec3 I = normalize(Position - cameraPos);//视角看量片元的方向
    //折射可以使用GLSL的内建refract函数来轻松实现，
    //它需要一个法向量、一个观察方向和两个材质之间的折射率(Refractive Index)。
    vec3 R = refract(I,normalize(Normal),ratio); // 使用refract函数计算折射向量
        
    FragColor = vec4(texture(skybox,R).rgb,1.0);

}