#version 330 core                                          
layout (location = 0) in vec3 aPos;                       
layout (location = 1) in vec3 aColors;     
layout (location = 2) in vec2 aTextureCoord; 

out vec4 vertexColor;//out  
out vec2 textureCoord;//out 
void main(){                                               
   gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0);           
   vertexColor = vec4(aColors.x,aColors.y,aColors.z,1.0f); 
   textureCoord = aTextureCoord;
}