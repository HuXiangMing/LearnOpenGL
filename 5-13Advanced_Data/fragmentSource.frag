#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

// ÎÆÀí²ÉÑù
uniform sampler2D ourTexture;

void main()
{   
        
    FragColor = texture(ourTexture, TexCoords);

}