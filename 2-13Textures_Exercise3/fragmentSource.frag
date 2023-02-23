#version 330 core                                  
in vec4 vertexColor;  
in vec2 textureCoord;
out vec4 FragColor; 

// �������
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

uniform float mixValue;

uniform bool isAwesomefaceHorizontalReversal = false;
uniform bool isAwesomefaceVerticalReversal = false;
void main(){    

	if(isAwesomefaceHorizontalReversal)
	{
		if(isAwesomefaceVerticalReversal)
		{
			//ˮƽ��ֱ����ת
			FragColor = mix(texture(ourTexture1, textureCoord), texture(ourTexture2, vec2(1-textureCoord.x,1-textureCoord.y)), mixValue)*vertexColor;//mix���Բ�ֵ����
		} 
		else
		{
			FragColor = mix(texture(ourTexture1, textureCoord), texture(ourTexture2, vec2(textureCoord.x,1-textureCoord.y)), mixValue)*vertexColor;//mix���Բ�ֵ����
		}
		
		//ˮƽ��תЦ��
		//����������֮�����Բ�ֵ(80% container, 20% awesomeface)
		
	}

	if(isAwesomefaceVerticalReversal)
	{
		if(isAwesomefaceHorizontalReversal)
		{
			//ˮƽ��ֱ����ת
			FragColor = mix(texture(ourTexture1, textureCoord), texture(ourTexture2, vec2(1-textureCoord.x,1-textureCoord.y)), mixValue)*vertexColor;//mix���Բ�ֵ����
		} 
		else
		{
			FragColor = mix(texture(ourTexture1, textureCoord), texture(ourTexture2, vec2(1-textureCoord.x,textureCoord.y)), mixValue)*vertexColor;//mix���Բ�ֵ����
		}		
	}

	if(!isAwesomefaceVerticalReversal && !isAwesomefaceHorizontalReversal)
	{
		//����ת
		//����������֮�����Բ�ֵ(80% container, 20% awesomeface)
		FragColor = mix(texture(ourTexture1, textureCoord), texture(ourTexture2, textureCoord), mixValue)*vertexColor;//mix���Բ�ֵ����
	}

}