#version 330 core                                  
in vec4 vertexColor;  
in vec2 textureCoord;
out vec4 FragColor; 

// �������
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main(){    

   //����������֮�����Բ�ֵ(80% container, 20% awesomeface)
   FragColor = mix(texture(ourTexture1, textureCoord), texture(ourTexture2, textureCoord), 0.2)*vertexColor;//mix���Բ�ֵ����
}