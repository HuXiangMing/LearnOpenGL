#version 330 core   
layout (location = 0) in vec2 aPos;  
layout (location = 1) in vec3 aCoords;
layout (location = 2) in vec2 aOffset; //实例化数组

out vec3 fColor; 

//uniform vec2 offsets[100];

void main(){  

	//在使用实例化渲染调用时，gl_InstanceID会从0开始，在每个实例被渲染时递增1
	//vec2 offset = offsets[gl_InstanceID];

	vec2 pos = aPos * (gl_InstanceID / 100.0);
	gl_Position = vec4(pos + aOffset, 0.0,1.0);
	fColor = aCoords;
	
}	