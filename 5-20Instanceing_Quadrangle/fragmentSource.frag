#version 330 core
out vec4 FragColor;

// ÎÆÀí²ÉÑù
in vec3 fColor;

void main()
{   
        
    FragColor = vec4(fColor, 1.0);

}