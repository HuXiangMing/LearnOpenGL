#ifndef MESH_H
#define MESH_H

/*

该Mesh用作测试，只负责用来创建立方体

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
/// 顶点
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

    Mesh_Cube(float vertices[]) //接受一组顶点数据测试MESH 立方体
    {
        this->vertices.resize(36);
        memcpy(&(this->vertices[0]), vertices, 36 * 8 * sizeof(float));
        setupMesh();
    }

    // 渲染网格
    void Draw(Shader &shader) 
    {        
        //  绘制网格
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);       
        glBindVertexArray(0);

        // 一旦配置，将所有内容设置回默认值始终是一个好习惯。
        glActiveTexture(GL_TEXTURE0);
    }

private:
    // 渲染数据
    unsigned int VBO, EBO;

    // 初始化缓冲区
    void setupMesh()
    {
        // 创建缓冲区
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        //glGenBuffers(1, &EBO);

        //绑定缓冲区->使得opengl接下来的操作都是针对于当前绑定的缓冲区
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        //struct的一个优点是它们的内存布局是顺序的。
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        // 设置顶点属性指针
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
