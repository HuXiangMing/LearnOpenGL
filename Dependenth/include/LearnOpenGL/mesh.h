#ifndef MESH_H
#define MESH_H

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <learnopengl/shader.h>

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

/// <summary>
/// ����
/// </summary>
struct Vertex {
    // ����λ�� position
    glm::vec3 Position;
    // ���� normal
    glm::vec3 Normal;
    //�������� texCoords
    glm::vec2 TexCoords;
    // ���� tangent
    glm::vec3 Tangent;
    // ������ bitangent �����ߣ������ߣ���ͬʱ��ֱ���ɷ��������ߵ����������Կ����ɷ��������ߵĲ������ó���
    glm::vec3 Bitangent;
    // bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    // weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};


/// <summary>
/// ����
/// </summary>
struct Texture {
    unsigned int id;     //����ID
    string type;         //�������ͣ���������ͼ�����Ǿ������ͼ��
    string path;         //������ͼ·��
};

class Mesh {
public:
    // ��������
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        
        setupMesh();
    }

    // ��Ⱦ����
    void Draw(Shader &shader) 
    {  
        // ������ÿ���������͵�N-������������ƴ�ӵ����������ַ����ϣ�����ȡ��Ӧ��uniform���ơ�
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // �ڰ�ǰ�����ʵ���������Ԫ
            // �����������(diffuse_textureN�е�N)
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // ��unsigned intת��Ϊ�ַ���
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // ��unsigned intת��Ϊ�ַ���
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // ��unsigned intת��Ϊ�ַ���

            // ������������Ϊ��ȷ��������Ԫ
            string uniformName = name + number;
            glUniform1i(glGetUniformLocation(shader.ID, uniformName.c_str()), i);

            // ������
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        //  ��������
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // һ�����ã��������������û�Ĭ��ֵʼ����һ����ϰ�ߡ�
        glActiveTexture(GL_TEXTURE0);
    }

private:
    // ��Ⱦ����
    unsigned int VBO, EBO;

    // ��ʼ��������
    void setupMesh()
    {
        // ����������
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        //�󶨻�����->ʹ��opengl�������Ĳ�����������ڵ�ǰ�󶨵Ļ�����
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        //C++�ṹ����һ���ܰ������ԣ����ǵ��ڴ沼����������(Sequential)��
        //Ҳ����˵��������ǽ��ṹ����Ϊһ����������ʹ�ã���ô��������˳�����нṹ��ı������⽫��ֱ��ת��Ϊ���������黺��������Ҫ��float��ʵ�������ֽڣ����顣
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


        // �ṹ�������һ���ܺõ���;������Ԥ����ָ��offsetof(s, m)�����ĵ�һ��������һ���ṹ�壬�ڶ�������������ṹ���б��������֡�
        // �����᷵���Ǹ�������ṹ��ͷ�����ֽ�ƫ����(Byte Offset)�������ÿ������ڶ���glVertexAttribPointer�����е�ƫ�Ʋ�����
        // ���ö�������ָ��
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
    }
};
#endif