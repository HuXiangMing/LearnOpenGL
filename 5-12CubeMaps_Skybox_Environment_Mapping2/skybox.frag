#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    //Ƭ����ɫ���ǳ�ֱ�ۡ����ǽ��������Ե�λ��������Ϊ����ķ�����������ʹ��������������ͼ�в�������ֵ��
    FragColor = texture(skybox, TexCoords);
} 