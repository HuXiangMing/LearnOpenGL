#version 330 core                                  
in vec4 vertexColor;  
in vec2 textureCoord;
out vec4 FragColor; 

// 纹理采样
uniform sampler2D ourTexture;

void main(){    
   // FragColor = vertexColor;  
   // FragColor = texture(ourTexture,textureCoord);//texture函数来采样纹理的颜色，它第一个参数是纹理采样器，第二个参数是对应的纹理坐标。
    FragColor = texture(ourTexture, textureCoord) * vertexColor;//纹理颜色与顶点颜色混合
   //FragColor = mix(texture(ourTexture, Texture), texture(ourTexture1, Texture), 0.9);//mix线性插值函数
}