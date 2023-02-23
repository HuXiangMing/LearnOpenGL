#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;  
void main()
{
	//�෴
//	FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);

	//�Ҷ�
//	FragColor = texture(screenTexture, TexCoords);
//    //float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
//	float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;//��Ȩ��(Weighted)ͨ��
//    FragColor = vec4(average, average, average, 1.0);

	//��Ч��
	vec2 offsets[9] = vec2[](//Ϊ��Χ���������괴����һ��9��vec2ƫ���������顣ƫ������һ������
        vec2(-offset,  offset), // ����
        vec2( 0.0f,    offset), // ����
        vec2( offset,  offset), // ����
        vec2(-offset,  0.0f),   // ��
        vec2( 0.0f,    0.0f),   // ��
        vec2( offset,  0.0f),   // ��
        vec2(-offset, -offset), // ����
        vec2( 0.0f,   -offset), // ����
        vec2( offset, -offset)  // ����
    );
    //��
//    float kernel[9] = float[](//����һ���ˣ��������������һ����(Sharpen)�ˣ����������Χ���������أ���ÿ����ɫֵ��
//        -1, -1, -1,
//        -1,  9, -1,
//        -1, -1, -1
//    );

    //ģ��
//	float kernel[9] = float[](//��������ֵ�ĺ���16������ֱ�ӷ��غϲ��Ĳ�����ɫ�������ǳ�������ɫ������������Ҫ���˵�ÿ��ֵ������16��
//    1.0 / 16, 2.0 / 16, 1.0 / 16,
//    2.0 / 16, 4.0 / 16, 2.0 / 16,
//    1.0 / 16, 2.0 / 16, 1.0 / 16  
//    );

    //��Ե���
	float kernel[9] = float[](//����˸��������еı�Ե�����������������֣�������ֻ����ͼ��ı߽ǵ�ʱ���Ƿǳ����õ�
           1, 1, 1,
           1,-8, 1,
           1, 1, 1
    );


    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        //�ڲ���ʱ���ǽ�ÿ��ƫ�����ӵ���ǰ����������
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    //����Щ����ֵ���Լ�Ȩ�ĺ�ֵ���������Ǽӵ�һ��
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    FragColor = vec4(col, 1.0);



} 