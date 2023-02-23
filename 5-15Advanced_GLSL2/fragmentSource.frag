#version 330 core
out vec4 FragColor;

//ʹ�ýӿڿ飬���ƽӿ��ᣬ���ԷŶ�����ݣ�
//ֻҪ�����ӿڿ������һ�������Ƕ�Ӧ��������������ƥ��������
in VS_OUT
{
    vec2 TexCoords;
} fs_in;
//in vec2 TexCoords;

uniform sampler2D frontTexture;
uniform sampler2D backTexture;

void main()
{
    //gl_FrontFacing�ܹ���֪��ǰƬ���������������һ���ֻ��Ǳ������һ����
    if(gl_FrontFacing)
        FragColor = texture(frontTexture, fs_in.TexCoords);
    else
        FragColor = texture(backTexture, fs_in.TexCoords);

}