#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform bool hdr;
uniform float exposure;

void main()
{     
    const float gamma =2.2;
    vec3 hdrColor = texture(hdrBuffer,TexCoords).rgb;

    if(hdr)
    {
        // Reinhard色调映射
        // vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
        // 曝光色调映射
        // 高曝光值会使隧道的黑暗部分显示更多的细节，然而低曝光值会显著减少黑暗区域的细节，但允许我们看到更多明亮区域的细节。
        vec3 result = vec3(1.0) - exp(-hdrColor * exposure);//高等数学里的以e为底的指数函数。
        result = pow(result ,vec3(1.0 / gamma));
        FragColor = vec4(result,1.0);
    }
    else
    {
        vec3 result = pow(hdrColor ,vec3(1.0/ gamma));
        FragColor = vec4(result,1.0);
    }
    
}
