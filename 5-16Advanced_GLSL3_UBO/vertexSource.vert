#version 330 core   
layout (location = 0) in vec3 aPos;  


//uniform块
layout (std140) uniform Matrices //矩阵
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;//模型矩阵会频繁变动,所以不封装在 uniform块中


void main(){  

	gl_Position = projection * view  * model * vec4(aPos, 1.0);  
}	