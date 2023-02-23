#version 330 core   
layout (location = 0) in vec3 aPos;  
layout (location = 1) in vec3 aNormal;//���㷨��

out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;

uniform vec3 lightPos;//lightPosĿǰ������ռ��С�

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){  

	gl_Position = projection * view * model * vec4(aPos, 1.0);      
	//ͨ���Ѷ���λ�����Գ���ģ�;��󣨲��ǹ۲��ͶӰ�����������任������ռ�����,�ٳ���view����תΪ�۲�ռ������
	FragPos = vec3(view * model * vec4(aPos, 1.0));
	//����ҲתΪ�۲�����
	Normal = mat3(transpose(inverse(view * model))) * aNormal;
	//�ƹ��λ��ҲתΪ�۲�����
	LightPos = vec3(view * vec4(lightPos, 1.0));
}	