#version 330 core   
layout (location = 0) in vec3 aPos;  
layout (location = 1) in vec2 aTexCoords;


//ʹ�ýӿڿ飬���ƽӿ��ᣬ���ԷŶ�����ݣ�
//ֻҪ�����ӿڿ������һ�������Ƕ�Ӧ��������������ƥ��������
out VS_OUT
{
    vec2 TexCoords;
} vs_out;

//out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){  

	gl_Position = projection * view  * model * vec4(aPos, 1.0);  
	vs_out.TexCoords = aTexCoords;
}	