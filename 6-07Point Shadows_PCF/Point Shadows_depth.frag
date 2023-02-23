#version 330 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    
    //计算的深度，这个深度就是每个fragment位置和光源位置之间的线性距离。
    float lightDistance = length(FragPos.xyz - lightPos);// 获取片元与光源之间的距离

    
    //把fragment和光源之间的距离，映射到0到1的范围，把它写入为fragment的深度值。
    lightDistance = lightDistance / far_plane;// 通过除远平面映射到[0;1]范围

    // 设置片元d额深度值
    gl_FragDepth = lightDistance;
}