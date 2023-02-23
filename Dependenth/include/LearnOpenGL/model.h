#ifndef MODEL_H
#define MODEL_H


#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION //使用stb_image.h 请先定义宏STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <learnopengl/mesh.h>
#include <learnopengl/shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory);

class Model
{
public:

    //模型数据
    vector<Texture> textures_loaded;//对材质进行缓存，方便重复利用
    vector<Mesh> meshes;
    string directory;


    Model(string path)
    {
        loadModel(path);
    }

    void Draw(Shader shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            meshes[i].Draw(shader);
        }
    }

private:   

    /// <summary>
    /// 从文件中加载一个Assimp扩展的模型，并进行解析储存为自身Mesh
    /// </summary>
    /// <param name="path">模型路径</param>
    void loadModel(string path)
    {
        // 使用Assimp读取模型
        // 
        // ReadFile函数，需要一个文件路径，它的第二个参数是一些后期处理(Post-processing)的选项
        // 
        // 通过设定aiProcess_Triangulate，我们告诉Assimp，如果模型不是（全部）由三角形组成，它需要将模型所有的图元形状变换为三角形。
        // aiProcess_CalcTangentSpace 计算导入网格的切线和双切线。
        // aiProcess_FlipUVs：将在处理的时候翻转y轴的纹理坐标（你可能还记得我们在纹理教程中说过，在OpenGL中大部分的图像的y轴都是反的，所以这个后期处理选项将会修复这个）
        // aiProcess_GenSmoothNormals：为网格中的所有顶点生成平滑法线。
        // aiProcess_GenNormals：如果模型不包含法向量的话，就为每个顶点创建法线。
        // aiProcess_SplitLargeMeshes：将比较大的网格分割成更小的子网格，如果你的渲染有最大顶点数限制，只能渲染较小的网格，那么它会非常有用。
        // aiProcess_OptimizeMeshes：和上个选项相反，它会将多个小网格拼接为一个大的网格，减少绘制调用从而进行优化。
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        // 检查加载是否有错误  GetErrorString函数来报告错误并返回
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }

        // 获取最后一个'/'  也就是文件路径
        directory = path.substr(0, path.find_last_of('/'));

        // 递归处理Assimp中的所有节点
        processNode(scene->mRootNode, scene);
    }

    /// <summary>
    /// 处理Assimp中的节点
    /// Assimp的结构中，每个节点包含了一系列的网格索引，每个索引指向场景对象中的那个特定网格。
    /// 因为每个节点（可能）包含有多个子节点，所以使用递归调用这个函数
    /// </summary>
    /// <param name="node">Assimp中的节点</param>
    /// <param name="scene">scene，Assimp数据接口的跟对象，通过这个场景对象，可以访问到加载后的模型中所有所需的数据</param>
    void processNode(aiNode* node, const aiScene* scene)
    {
        //先处理当前节点下的所有Mesh，并转换成我们自己的Mesh结构储存起来
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        //处理所有网格后，在处理子节点
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    /// <summary>
    /// 将Assimp的aiMesh转换成自己的Mesh
    /// </summary>
    /// <param name="mesh">Assimp的aiMesh</param>
    /// <param name="scene">scene，Assimp数据接口的跟对象，通过这个场景对象，可以访问到加载后的模型中所有所需的数据</param>
    /// <returns>自己的Mesh结构</returns>
    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // 构建Mesh的必要数据
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        //遍历网格的所有顶点
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            // 顶点位置 position
            glm::vec3 vector; //临时变量，因为Assimp对向量、矩阵、字符串等都有自己的一套数据类型，它们并不能完美地转换到GLM的数据类型中
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // 法线 normal
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            
            if (mesh->mTextureCoords[0])//是否包含纹理坐标
            {
                glm::vec2 vec;
                // 纹理坐标 texCoords
                // 一个顶点最多可以包含8个不同的纹理坐标。但我们不会用到这么多
                // 我们只关心第一组纹理坐标，所以我们总是取第一个集合(0)。
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // 切线 tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // 副切线 bitangent  
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }


        // 遍历网格的所有面，每个面代表了一个图元，它总是三角形。一个面包含了多个索引
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // 遍历顶点索引并储存
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }


        // 遍历网格中的所有材质，一个网格只包含了一个指向材质对象的索引。
        // 如果想要获取网格真正的材质，我们还需要索引场景的mMaterials数组。
        // 网格材质索引位于它的mMaterialIndex属性中
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            // 我们约定好着色器中的采样器名称
            // 每个漫反射纹理都应该命名例如'texture_diffuseN'
            // 其中N是一个从1到MAX_SAMPLER_NUMBER的连续数字。
            // 同样适用于其他纹理，如下列表总结:
            // diffuse: texture_diffuseN
            // specular: texture_specularN
            // normal: texture_normalN


            // 1. diffuse maps
            vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. specular maps
            vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. normal maps
            std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. height maps
            std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

            // 返回从提取的网格数据创建的网格对象
            return Mesh(vertices, indices, textures);
        }
    }


    /// <summary>
    /// 将Assimp的aiMaterial转为我们自己的Texture
    /// 一个材质对象的内部对每种纹理类型都存储了一个纹理位置数组。不同的纹理类型都以aiTextureType_为前缀
    /// </summary>
    /// <param name="mat">Assimp的材质</param>
    /// <param name="type">材质类型</param>
    /// <param name="typeName">材质类型名称</param>
    /// <returns></returns>
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;

        //遍历Assimp的纹理
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);// GetTexture获取每个纹理的文件位置，结果储存在一个aiString中

            // 检查之前是否加载了纹理，如果是，继续下一次迭代:跳过加载一个新的纹理
            bool skip = false;

            //遍历当前加载的所有纹理，检查是否有与当前正在加载的纹理相同的情况
            //如果有则使用加载好的纹理，否则进行纹理加载
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                //strcmp用于比较两个字符串并根据比较结果返回整数，若str1=str2，则返回零；若str1<str2，则返回负数；若str1>str2，则返回正数。
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    //该纹理已经加载
                    textures.push_back(textures_loaded[j]);
                    skip = true; 
                    break;
                }
            }

            // 如果纹理还没有加载，则进行纹理加载
            if (!skip)
            {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);//进行缓存，方便重复利用
            }

        }

        return textures;
    }
};


unsigned int TextureFromFile(const char* path, const string& directory)
{
    string filename = string(path);
    filename = directory + '/' + filename;
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
#endif