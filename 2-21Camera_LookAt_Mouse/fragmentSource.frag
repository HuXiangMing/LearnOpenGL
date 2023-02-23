#version 330 core                                   
in vec2 textureCoord;
out vec4 FragColor; 

// 纹理采样
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main(){    

   //在两个纹理之间线性插值(80% container, 20% awesomeface)
   FragColor = mix(texture(ourTexture1, textureCoord), texture(ourTexture2, textureCoord), 0.2);//mix线性插值函数
}