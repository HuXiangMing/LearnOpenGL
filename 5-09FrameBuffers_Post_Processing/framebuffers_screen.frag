#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;  
void main()
{
	//相反
//	FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);

	//灰度
//	FragColor = texture(screenTexture, TexCoords);
//    //float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
//	float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;//加权的(Weighted)通道
//    FragColor = vec4(average, average, average, 1.0);

	//核效果
	vec2 offsets[9] = vec2[](//为周围的纹理坐标创建了一个9个vec2偏移量的数组。偏移量是一个常量
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );
    //锐化
//    float kernel[9] = float[](//定义一个核，在这个例子中是一个锐化(Sharpen)核，它会采样周围的所有像素，锐化每个颜色值。
//        -1, -1, -1,
//        -1,  9, -1,
//        -1, -1, -1
//    );

    //模糊
//	float kernel[9] = float[](//由于所有值的和是16，所以直接返回合并的采样颜色将产生非常亮的颜色，所以我们需要将核的每个值都除以16。
//    1.0 / 16, 2.0 / 16, 1.0 / 16,
//    2.0 / 16, 4.0 / 16, 2.0 / 16,
//    1.0 / 16, 2.0 / 16, 1.0 / 16  
//    );

    //边缘检测
	float kernel[9] = float[](//这个核高亮了所有的边缘，而暗化了其它部分，在我们只关心图像的边角的时候是非常有用的
           1, 1, 1,
           1,-8, 1,
           1, 1, 1
    );


    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        //在采样时我们将每个偏移量加到当前纹理坐标上
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    //将这些纹理值乘以加权的核值，并将它们加到一起。
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    FragColor = vec4(col, 1.0);



} 