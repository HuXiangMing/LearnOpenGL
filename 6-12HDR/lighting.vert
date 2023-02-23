#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoods;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform bool inverse_normals;//是否反转法线

void main()
{
    vs_out.FragPos = vec3(model*vec4(aPos,1.0));
    vs_out.TexCoods = aTexCoords;

    vec3 n = inverse_normals ? -aNormal : aNormal;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.Normal = normalize(normalMatrix * n);    
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
   
}