#version 330 core   
layout (location = 0) in vec3 aPos;  
layout (location = 1) in vec2 aTexCoords;


//使用接口块，类似接口提，可以放多个数据，
//只要两个接口块的名字一样，它们对应的输入和输出将会匹配起来。
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