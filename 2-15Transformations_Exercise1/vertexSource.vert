#version 330 core                                          
layout (location = 0) in vec3 aPos;                          
layout (location = 1) in vec2 aTextureCoord; 

uniform mat4 transform;

out vec4 vertexColor;
out vec2 textureCoord; 

void main(){  

   gl_Position = transform * vec4(aPos, 1.0f);          
   textureCoord = aTextureCoord;
}