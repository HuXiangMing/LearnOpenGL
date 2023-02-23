#version 330 core
out vec4 FragColor;

//使用接口块，类似接口提，可以放多个数据，
//只要两个接口块的名字一样，它们对应的输入和输出将会匹配起来。
in VS_OUT
{
    vec2 TexCoords;
} fs_in;
//in vec2 TexCoords;

uniform sampler2D frontTexture;
uniform sampler2D backTexture;

void main()
{
    //gl_FrontFacing能够得知当前片段是属于正向面的一部分还是背向面的一部分
    if(gl_FrontFacing)
        FragColor = texture(frontTexture, fs_in.TexCoords);
    else
        FragColor = texture(backTexture, fs_in.TexCoords);

}