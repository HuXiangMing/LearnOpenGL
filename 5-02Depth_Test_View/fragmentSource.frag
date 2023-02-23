#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

//相机投影参数
float near = 0.1; 
float far  = 100.0; 


//将屏幕空间深度值的非线性，转为线性
float LinearizeDepth(float depth) 
{
    //首先将深度值变换为NDC(从【0-1】范围重新变换到【-1，1】范围)
    float z = depth * 2.0 - 1.0;
    //获取到的z值后，应用逆变换来获取线性的深度值：
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{    
    //gl_FragCoord向量的Z值包含了那个特定片段的深度值
    //当靠近片元时，Z值会逐渐缩小，显示为黑色
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // 将线性深度值除以far，我们近似地将线性深度值转化到[0, 1]的范围之间。
    FragColor = vec4(vec3(depth), 1.0);
}