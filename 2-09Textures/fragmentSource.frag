#version 330 core                                  
in vec4 vertexColor;  
in vec2 textureCoord;
out vec4 FragColor; 

// �������
uniform sampler2D ourTexture;

void main(){    
   // FragColor = vertexColor;  
   // FragColor = texture(ourTexture,textureCoord);//texture�����������������ɫ������һ��������������������ڶ��������Ƕ�Ӧ���������ꡣ
    FragColor = texture(ourTexture, textureCoord) * vertexColor;//������ɫ�붥����ɫ���
   //FragColor = mix(texture(ourTexture, Texture), texture(ourTexture1, Texture), 0.9);//mix���Բ�ֵ����
}