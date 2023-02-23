#version 330 core   
layout (location = 0) in vec3 aPos;  
layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 normal;
} vs_out;

//uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){ 

	//为了适配（观察和模型矩阵的）缩放和旋转，我们在将法线变换到观察空间坐标之前
	gl_Position =  view * model * vec4(aPos, 1.0); 
	mat3 normalMatrix = mat3(transpose(inverse(view * model)));
	vs_out.normal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
	
}	