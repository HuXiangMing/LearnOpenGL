#version 330 core
out vec4 FragColor;


void main()
{
    //gl_FragCoord��x��y������Ƭ�εĴ��ڿռ�(Window-space)���꣬��ԭ��Ϊ���ڵ����½ǡ�
    if(gl_FragCoord.x < 400)
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    else
        FragColor = vec4(0.0, 1.0, 0.0, 1.0); 

}