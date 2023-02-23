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
/// 顶点
/// </summary>
struct Vertex {
    // 顶点位置 position
    glm::vec3 Position;
    // 法线 normal
    glm::vec3 Normal;
    //纹理坐标 texCoords
    glm::vec2 TexCoords;
    // 切线 tangent
    glm::vec3 Tangent;
    // 副切线 bitangent 副切线（副法线）是同时垂直于由法线与切线的向量，所以可以由法线与切线的叉积计算得出。
    glm::vec3 Bitangent;
    // bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    // weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};


/// <summary>
/// 纹理
/// </summary>
struct Texture {
    unsigned int id;     //纹理ID
    string type;         //纹理类型（漫反射贴图或者是镜面光贴图）
    string path;         //纹理贴图路径
};

class Mesh {
public:
    // 网格数据
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

    // 渲染网格
    void Draw(Shader &shader) 
    {  
        // 计算了每个纹理类型的N-分量，并将其拼接到纹理类型字符串上，来获取对应的uniform名称。
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // 在绑定前激活适当的纹理单元
            // 检索纹理编号(diffuse_textureN中的N)
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // 将unsigned int转换为字符串
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // 将unsigned int转换为字符串
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // 将unsigned int转换为字符串

            // 将采样器设置为正确的纹理单元
            string uniformName = name + number;
            glUniform1i(glGetUniformLocation(shader.ID, uniformName.c_str()), i);

            // 绑定纹理
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        //  绘制网格
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
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
        glGenBuffers(1, &EBO);

        //绑定缓冲区->使得opengl接下来的操作都是针对于当前绑定的缓冲区
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        //C++结构体有一个很棒的特性，它们的内存布局是连续的(Sequential)。
        //也就是说，如果我们将结构体作为一个数据数组使用，那么它将会以顺序排列结构体的变量，这将会直接转换为我们在数组缓冲中所需要的float（实际上是字节）数组。
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


        // 结构体的另外一个很好的用途是它的预处理指令offsetof(s, m)，它的第一个参数是一个结构体，第二个参数是这个结构体中变量的名字。
        // 这个宏会返回那个变量距结构体头部的字节偏移量(Byte Offset)。这正好可以用在定义glVertexAttribPointer函数中的偏移参数：
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
