#version 330 core   
layout (location = 0) in vec3 aPos;  
layout (location = 1) in vec3 aNormal;//顶点法线

out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;

uniform vec3 lightPos;//lightPos目前在世界空间中。

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){  

	gl_Position = projection * view * model * vec4(aPos, 1.0);      
	//通过把顶点位置属性乘以模型矩阵（不是观察和投影矩阵）来把它变换到世界空间坐标,再乘以view矩阵转为观察空间的坐标
	FragPos = vec3(view * model * vec4(aPos, 1.0));
	//法线也转为观察坐标
	Normal = mat3(transpose(inverse(view * model))) * aNormal;
	//灯光的位置也转为观察坐标
	LightPos = vec3(view * vec4(lightPos, 1.0));
}	