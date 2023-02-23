#ifndef MESH_H
#define MESH_H

/*

��Mesh�������ԣ�ֻ������������������

*/


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
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
};


class Mesh_Cube {
public:

    vector<Vertex>       vertices;
    unsigned int VAO;

    Mesh_Cube(float vertices[]) //����һ�鶥�����ݲ���MESH ������
    {
        this->vertices.resize(36);
        memcpy(&(this->vertices[0]), vertices, 36 * 8 * sizeof(float));
        setupMesh();
    }

    // ��Ⱦ����
    void Draw(Shader &shader) 
    {        
        //  ��������
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);       
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
        //glGenBuffers(1, &EBO);

        //�󶨻�����->ʹ��opengl�������Ĳ�����������ڵ�ǰ�󶨵Ļ�����
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        //struct��һ���ŵ������ǵ��ڴ沼����˳��ġ�
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

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

        glBindVertexArray(0);
    }
};
#endif
