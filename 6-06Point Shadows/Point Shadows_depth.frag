#version 330 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    
    //�������ȣ������Ⱦ���ÿ��fragmentλ�ú͹�Դλ��֮������Ծ��롣
    float lightDistance = length(FragPos.xyz - lightPos);// ��ȡƬԪ���Դ֮��ľ���

    
    //��fragment�͹�Դ֮��ľ��룬ӳ�䵽0��1�ķ�Χ������д��Ϊfragment�����ֵ��
    lightDistance = lightDistance / far_plane;// ͨ����Զƽ��ӳ�䵽[0;1]��Χ

    // ����ƬԪd�����ֵ
    gl_FragDepth = lightDistance;
}