#version 330 core                                  
in vec4 vertexColor;  
in vec2 textureCoord;
out vec4 FragColor; 

// 纹理采样
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
			//水平垂直都翻转
			FragColor = mix(texture(ourTexture1, textureCoord), texture(ourTexture2, vec2(1-textureCoord.x,1-textureCoord.y)), mixValue)*vertexColor;//mix线性插值函数
		} 
		else
		{
			FragColor = mix(texture(ourTexture1, textureCoord), texture(ourTexture2, vec2(textureCoord.x,1-textureCoord.y)), mixValue)*vertexColor;//mix线性插值函数
		}
		
		//水平翻转笑脸
		//在两个纹理之间线性插值(80% container, 20% awesomeface)
		
	}

	if(isAwesomefaceVerticalReversal)
	{
		if(isAwesomefaceHorizontalReversal)
		{
			//水平垂直都翻转
			FragColor = mix(texture(ourTexture1, textureCoord), texture(ourTexture2, vec2(1-textureCoord.x,1-textureCoord.y)), mixValue)*vertexColor;//mix线性插值函数
		} 
		else
		{
			FragColor = mix(texture(ourTexture1, textureCoord), texture(ourTexture2, vec2(1-textureCoord.x,textureCoord.y)), mixValue)*vertexColor;//mix线性插值函数
		}		
	}

	if(!isAwesomefaceVerticalReversal && !isAwesomefaceHorizontalReversal)
	{
		//不翻转
		//在两个纹理之间线性插值(80% container, 20% awesomeface)
		FragColor = mix(texture(ourTexture1, textureCoord), texture(ourTexture2, textureCoord), mixValue)*vertexColor;//mix线性插值函数
	}

}