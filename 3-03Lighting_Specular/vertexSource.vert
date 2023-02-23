#version 330 core   
layout (location = 0) in vec3 aPos;  
layout (location = 1) in vec3 aNormal;//���㷨��

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){  

	//ͨ���Ѷ���λ�����Գ���ģ�;��󣨲��ǹ۲��ͶӰ�����������任������ռ�����
	FragPos = vec3(model * vec4(aPos, 1.0));
	//Normal = aNormal;����������model������������ת�����ţ���ʱ�ķ���ȴ����ԭ�������ݣ�������������Ч��
	Normal = mat3(transpose(inverse(model))) * aNormal;	//ʹ��inverse��transpose����������߾���
	gl_Position = projection * view * vec4(FragPos, 1.0);      
	
}	