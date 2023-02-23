#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;

void main()
{
    //������ɫ���򵥵ؽ�����任������ռ䣬Ȼ��ֱ�ӷ��͵�������ɫ����
    gl_Position = model * vec4(aPos, 1.0);
}