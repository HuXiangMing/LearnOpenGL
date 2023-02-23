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

#define STB_IMAGE_IMPLEMENTATION //ʹ��stb_image.h ���ȶ����STB_IMAGE_IMPLEMENTATION
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

    //ģ������
    vector<Texture> textures_loaded;//�Բ��ʽ��л��棬�����ظ�����
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
    /// ���ļ��м���һ��Assimp��չ��ģ�ͣ������н�������Ϊ����Mesh
    /// </summary>
    /// <param name="path">ģ��·��</param>
    void loadModel(string path)
    {
        // ʹ��Assimp��ȡģ��
        // 
        // ReadFile��������Ҫһ���ļ�·�������ĵڶ���������һЩ���ڴ���(Post-processing)��ѡ��
        // 
        // ͨ���趨aiProcess_Triangulate�����Ǹ���Assimp�����ģ�Ͳ��ǣ�ȫ��������������ɣ�����Ҫ��ģ�����е�ͼԪ��״�任Ϊ�����Ρ�
        // aiProcess_CalcTangentSpace ���㵼����������ߺ�˫���ߡ�
        // aiProcess_FlipUVs�����ڴ����ʱ��תy����������꣨����ܻ��ǵ�����������̳���˵������OpenGL�д󲿷ֵ�ͼ���y�ᶼ�Ƿ��ģ�����������ڴ���ѡ����޸������
        // aiProcess_GenSmoothNormals��Ϊ�����е����ж�������ƽ�����ߡ�
        // aiProcess_GenNormals�����ģ�Ͳ������������Ļ�����Ϊÿ�����㴴�����ߡ�
        // aiProcess_SplitLargeMeshes�����Ƚϴ������ָ�ɸ�С����������������Ⱦ����󶥵������ƣ�ֻ����Ⱦ��С��������ô����ǳ����á�
        // aiProcess_OptimizeMeshes�����ϸ�ѡ���෴�����Ὣ���С����ƴ��Ϊһ��������񣬼��ٻ��Ƶ��ôӶ������Ż���
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        // �������Ƿ��д���  GetErrorString������������󲢷���
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }

        // ��ȡ���һ��'/'  Ҳ�����ļ�·��
        directory = path.substr(0, path.find_last_of('/'));

        // �ݹ鴦��Assimp�е����нڵ�
        processNode(scene->mRootNode, scene);
    }

    /// <summary>
    /// ����Assimp�еĽڵ�
    /// Assimp�Ľṹ�У�ÿ���ڵ������һϵ�е�����������ÿ������ָ�򳡾������е��Ǹ��ض�����
    /// ��Ϊÿ���ڵ㣨���ܣ������ж���ӽڵ㣬����ʹ�õݹ�����������
    /// </summary>
    /// <param name="node">Assimp�еĽڵ�</param>
    /// <param name="scene">scene��Assimp���ݽӿڵĸ�����ͨ������������󣬿��Է��ʵ����غ��ģ�����������������</param>
    void processNode(aiNode* node, const aiScene* scene)
    {
        //�ȴ���ǰ�ڵ��µ�����Mesh����ת���������Լ���Mesh�ṹ��������
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        //��������������ڴ����ӽڵ�
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    /// <summary>
    /// ��Assimp��aiMeshת�����Լ���Mesh
    /// </summary>
    /// <param name="mesh">Assimp��aiMesh</param>
    /// <param name="scene">scene��Assimp���ݽӿڵĸ�����ͨ������������󣬿��Է��ʵ����غ��ģ�����������������</param>
    /// <returns>�Լ���Mesh�ṹ</returns>
    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // ����Mesh�ı�Ҫ����
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        //������������ж���
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            // ����λ�� position
            glm::vec3 vector; //��ʱ��������ΪAssimp�������������ַ����ȶ����Լ���һ���������ͣ����ǲ�����������ת����GLM������������
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // ���� normal
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            
            if (mesh->mTextureCoords[0])//�Ƿ������������
            {
                glm::vec2 vec;
                // �������� texCoords
                // һ�����������԰���8����ͬ���������ꡣ�����ǲ����õ���ô��
                // ����ֻ���ĵ�һ���������꣬������������ȡ��һ������(0)��
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // ���� tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // ������ bitangent  
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


        // ��������������棬ÿ���������һ��ͼԪ�������������Ρ�һ��������˶������
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // ������������������
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }


        // ���������е����в��ʣ�һ������ֻ������һ��ָ����ʶ����������
        // �����Ҫ��ȡ���������Ĳ��ʣ����ǻ���Ҫ����������mMaterials���顣
        // �����������λ������mMaterialIndex������
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            // ����Լ������ɫ���еĲ���������
            // ÿ������������Ӧ����������'texture_diffuseN'
            // ����N��һ����1��MAX_SAMPLER_NUMBER���������֡�
            // ͬ���������������������б��ܽ�:
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

            // ���ش���ȡ���������ݴ������������
            return Mesh(vertices, indices, textures);
        }
    }


    /// <summary>
    /// ��Assimp��aiMaterialתΪ�����Լ���Texture
    /// һ�����ʶ�����ڲ���ÿ���������Ͷ��洢��һ������λ�����顣��ͬ���������Ͷ���aiTextureType_Ϊǰ׺
    /// </summary>
    /// <param name="mat">Assimp�Ĳ���</param>
    /// <param name="type">��������</param>
    /// <param name="typeName">������������</param>
    /// <returns></returns>
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;

        //����Assimp������
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);// GetTexture��ȡÿ��������ļ�λ�ã����������һ��aiString��

            // ���֮ǰ�Ƿ��������������ǣ�������һ�ε���:��������һ���µ�����
            bool skip = false;

            //������ǰ���ص�������������Ƿ����뵱ǰ���ڼ��ص�������ͬ�����
            //�������ʹ�ü��غõ�������������������
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                //strcmp���ڱȽ������ַ��������ݱȽϽ��������������str1=str2���򷵻��㣻��str1<str2���򷵻ظ�������str1>str2���򷵻�������
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    //�������Ѿ�����
                    textures.push_back(textures_loaded[j]);
                    skip = true; 
                    break;
                }
            }

            // �������û�м��أ�������������
            if (!skip)
            {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);//���л��棬�����ظ�����
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