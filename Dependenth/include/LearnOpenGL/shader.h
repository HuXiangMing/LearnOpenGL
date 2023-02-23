#pragma once


#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class Shader
{
public:
	unsigned int ID;
	string vertexString;
	string fragmentString;
	string geometryString;
	const char* vertexSource;
	const char* fragmentSource;
	const char* geometrySource;

	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
	{
		//1。从filePath中检索顶点/片段源代码
		ifstream vertexFile;
		ifstream fragmentFile;
		ifstream geometryFile;
		stringstream vertexSStream;
		stringstream fragmentSStream;
		stringstream geometrySStream;


		vertexFile.open(vertexPath);
		fragmentFile.open(fragmentPath);
		if (geometryPath != nullptr)
			geometryFile.open(geometryPath);

		//确保ifstream对象可以抛出异常:
		vertexFile.exceptions(ifstream::failbit || ifstream::badbit);
		fragmentFile.exceptions(ifstream::failbit || ifstream::badbit);
		geometryFile.exceptions(ifstream::failbit || ifstream::badbit);

		try
		{
			//着色器文件是否可以正常打开
			if (!vertexFile.is_open() || !fragmentFile.is_open())
			{
				throw exception("文件打开失败");
			}

			//读取文件的缓冲区内容到字符流
			vertexSStream << vertexFile.rdbuf();
			fragmentSStream << fragmentFile.rdbuf();

			// 关闭文件处理
			vertexFile.close();
			fragmentFile.close();

			//将字符流转为字符串
			vertexString = vertexSStream.str();
			fragmentString = fragmentSStream.str();


			//如果存在几何着色器则，处理几何着色器
			if (geometryPath != nullptr)
			{
				geometrySStream << geometryFile.rdbuf();
				geometryFile.close();
				geometryString = geometrySStream.str();
			}

		}
		catch (const std::exception& ex)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << ex.what() << std::endl;
		}

		vertexSource = vertexString.c_str();
		fragmentSource = fragmentString.c_str();
		geometrySource = geometryString.c_str();

		cout << vertexSource << endl;
		cout << fragmentSource << endl;
		cout << geometrySource << endl;

		// 2. 编译着色器
		unsigned int vertex, fragment;

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexSource, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragmentSource, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");

		//如果存在几何着色器则，处理几何着色器
		unsigned int geometry;
		if (geometryPath != nullptr)
		{
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &geometrySource, NULL);
			glCompileShader(geometry);
			checkCompileErrors(geometry, "GEOMETRY");
		}

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geometryPath != nullptr)
			glAttachShader(ID, geometry);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

		//链接完成就不再需要了
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryPath != nullptr)
			glDeleteShader(geometry);
	};







	// 激活着色器
	// ------------------------------------------------------------------------
	void use()
	{
		glUseProgram(ID);
	}
	// glUniform1函数封装
	// ------------------------------------------------------------------------
	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setVec2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string& name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

private:
	//检查shader的源码是否有错误
	void checkCompileErrors(unsigned int id, string type)
	{
		int success;//成功
		char infoLog[1024]{};

		if (type != "PROGRAM")
		{
			//glGetShaderiv用来检测着色器编译是否成功。
			glGetShaderiv(id, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(ID, sizeof(infoLog) / sizeof(infoLog[0]), NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			//glGetProgramiv用来检查链接是否成功。
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(ID, sizeof(infoLog) / sizeof(infoLog[0]), NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}

		}
	}
};


