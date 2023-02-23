#version 330 core   
layout (location = 0) in vec3 aPos;  
layout (location = 1) in vec3 aNormal;//顶点法线

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){  

	//通过把顶点位置属性乘以模型矩阵（不是观察和投影矩阵）来把它变换到世界空间坐标
	FragPos = vec3(model * vec4(aPos, 1.0));
	//Normal = aNormal;由于我们在model矩阵中做了旋转或缩放，此时的法线却还是原来的数据，将会产生以外的效果
	Normal = mat3(transpose(inverse(model))) * aNormal;	//使用inverse和transpose函数这个法线矩阵
	gl_Position = projection * view * vec4(FragPos, 1.0);      
	
}	