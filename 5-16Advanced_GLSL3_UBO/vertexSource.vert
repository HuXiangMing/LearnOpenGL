#version 330 core   
layout (location = 0) in vec3 aPos;  


//uniform��
layout (std140) uniform Matrices //����
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;//ģ�;����Ƶ���䶯,���Բ���װ�� uniform����


void main(){  

	gl_Position = projection * view  * model * vec4(aPos, 1.0);  
}	