#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;

void main()
{
    //顶点着色器简单地将顶点变换到世界空间，然后直接发送到几何着色器：
    gl_Position = model * vec4(aPos, 1.0);
}