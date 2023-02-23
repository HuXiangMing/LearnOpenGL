// 5-20Instanceing_Quadrangle : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

/*
实例化_四边形

应用：使用glDrawArraysInstanced和glDrawElementsInstanced同时绘制100个四边形

假设你有一个绘制了很多模型的场景，而大部分的模型包含的是同一组顶点数据，只不过进行的是不同的世界空间变换。
想象一个充满草的场景：每根草都是一个包含几个三角形的小模型。你可能会需要绘制很多根草，最终在每帧中你可能会需要渲染上千或者上万根草。
因为每一根草仅仅是由几个三角形构成，渲染几乎是瞬间完成的，但上千个渲染函数调用却会极大地影响性能。

如果我们需要渲染大量物体时，代码看起来会像这样：

for(unsigned int i = 0; i < amount_of_models_to_draw; i++)
{
	DoSomePreparations(); // 绑定VAO，绑定纹理，设置uniform等
	glDrawArrays(GL_TRIANGLES, 0, amount_of_vertices);
}

glDrawArrays或glDrawElements的性能瓶颈
如果像这样绘制模型的大量实例(Instance)，你很快就会因为绘制调用过多而达到性能瓶颈。
与绘制顶点本身相比，使用glDrawArrays或glDrawElements函数告诉GPU去绘制你的顶点数据会消耗更多的性能，

因为OpenGL在绘制顶点数据之前需要做很多准备工作（比如告诉GPU该从哪个缓冲读取数据，
从哪寻找顶点属性，而且这些都是在相对缓慢的CPU到GPU总线(CPU to GPU Bus)上进行的）。
所以，即便渲染顶点非常快，命令GPU去渲染却未必。

如果我们能够将数据一次性发送给GPU，然后使用一个绘制函数让OpenGL利用这些数据绘制多个物体，就会更方便了。这就是实例化(Instancing)。

glDrawArrays和glDrawElements
实例化这项技术能够让我们使用一个渲染调用来绘制多个物体，来节省每次绘制物体时CPU -> GPU的通信，它只需要一次即可。
如果想使用实例化渲染，我们只需要将glDrawArrays和glDrawElements的渲染调用分别改为glDrawArraysInstanced和glDrawElementsInstanced就可以了。
这些渲染函数的实例化版本需要一个额外的参数，叫做实例数量(Instance Count)，它能够设置我们需要渲染的实例个数。
这样我们只需要将必须的数据发送到GPU一次，然后使用一次函数调用告诉GPU它应该如何绘制这些实例。
GPU将会直接渲染这些实例，而不用不断地与CPU进行通信。

gl_InstanceID。
这个函数本身并没有什么用。渲染同一个物体一千次对我们并没有什么用处，每个物体都是完全相同的，
而且还在同一个位置。我们只能看见一个物体！处于这个原因，GLSL在顶点着色器中嵌入了另一个内建变量，gl_InstanceID。
在使用实例化渲染调用时，gl_InstanceID会从0开始，在每个实例被渲染时递增1。
比如说，我们正在渲染第43个实例，那么顶点着色器中它的gl_InstanceID将会是42。
因为每个实例都有唯一的ID，我们可以建立一个数组，将ID与位置值对应起来，将每个实例放置在世界的不同位置。

了体验一下实例化绘制，我们将会在标准化设备坐标系中使用一个渲染调用，绘制100个2D四边形。
我们会索引一个包含100个偏移向量的uniform数组，将偏移值加到每个实例化的四边形上。
最终的结果是一个排列整齐的四边形网格


实例化数组
uniform数据大小上限
虽然之前的实现在目前的情况下能够正常工作，但是如果我们要渲染远超过100个实例的时候（这其实非常普遍），
我们最终会超过最大能够发送至着色器的uniform数据大小上限。它的一个代替方案是实例化数组(Instanced Array)，
它被定义为一个顶点属性（能够让我们储存更多的数据），仅在顶点着色器渲染一个新的实例时才会更新。

使用顶点属性时，顶点着色器的每次运行都会让GLSL获取新一组适用于当前顶点的属性。
而当我们将顶点属性定义为一个实例化数组时，顶点着色器就只需要对每个实例，而不是每个顶点，
更新顶点属性的内容了。这允许我们对逐顶点的数据使用普通的顶点属性，而对逐实例的数据使用实例化数组。

我们不再使用gl_InstanceID，现在不需要索引一个uniform数组就能够直接使用offset属性了。

glVertexAttribDivisor
这个函数告诉了OpenGL该什么时候更新顶点属性的内容至新一组数据。
它的第一个参数是需要的顶点属性，第二个参数是属性除数(Attribute Divisor)。
默认情况下，属性除数是0，告诉OpenGL我们需要在顶点着色器的每次迭代时更新顶点属性。
将它设置为1时，我们告诉OpenGL我们希望在渲染一个新实例的时候更新顶点属性。
而设置为2时，我们希望每2个实例更新一次属性，以此类推。
我们将属性除数设置为1，是在告诉OpenGL，处于位置值2的顶点属性是一个实例化数组。
*/


#include <iostream>

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <LearnOpenGL/shader.h>
#include <LearnOpenGL/camera.h>
#include <LearnOpenGL/model.h>
#include <LearnOpenGL/filesystem.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>




void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path);
unsigned int loadCubemap(vector<std::string> faces);

//屏幕宽高
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//摄像机参数
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//Deltatime
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间


int main()
{
	// glfw: 初始化
	// ------------------------------
	glfwInit();//初始化glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//主版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//次版本号
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//早期是固定流水线，现在是可编程话流水线

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


	// glfw: 创建窗口
	// ------------------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "5-20Instanceing_Quadrangle", NULL, NULL);
	if (window == NULL)
	{
		printf("窗口创建失败");
		glfwTerminate();//终止掉glfw
		return -1;
	}

	glfwMakeContextCurrent(window);//设置为glfw的主窗口 
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//隐藏光标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glew:加载所有OpenGL函数指针
	// -----------------------------
	glewExperimental = true;//GLEW在管理OpenGL的函数指针时更多地使用现代化的技术。
	if (glewInit() != GLEW_OK)
	{
		printf("glew初始化失败");
		return -1;
	}

	//配置全局opengl状态
	// ------------------------------------
	glEnable(GL_DEPTH_TEST);//启用深度测试	


	// glew加载完成后构建和编译着色器程序
	// ------------------------------------
	Shader shader("vertexSource.vert", "fragmentSource.frag");

	//生成100个四坐标位置/平移向量的列表
	// ------------------------------------
	glm::vec2 translations[100];
	int index = 0;
	float offset = 0.1f;
	for (int y = -10; y < 10; y += 2)
	{
		for (int x = -10; x < 10; x += 2)
		{
			glm::vec2 translation;
			translation.x = (float)x / 10.0f + offset;
			translation.y = (float)y / 10.0f + offset;
			translations[index++] = translation;
		}
	}

#pragma region 处理渲染数据，绘制指令等

	float quadVertices[] = {
		// 位置          // 颜色
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};

	//cube 
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	//分配好内存后，将data设置为NULL,之后可以使用glBufferSubData来分段填充
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2*sizeof(float)));

	//在数组缓冲区中存储实例数据
	unsigned int instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//同时设置实例数据
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // 该属性来自不同的顶点缓冲区
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(2, 1); // 告诉OpenGL这是一个实例顶点属性。

#pragma endregion


#pragma region 加载贴图

	//unsigned int cubeTexture = loadTexture(FileSystem::getPath("resources/textures/container.jpg").c_str());

#pragma endregion


#pragma region shader配置

	shader.use();

#pragma endregion


#pragma region 帧缓冲配置
	
#pragma endregion
	// 渲染循环
	// -----------
	while (!glfwWindowShouldClose(window))
	{

		//Deltatime
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 处理输入
		// -----------
		processInput(window);

#pragma region Draw

		// 处理渲染
		// -----------		
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//在设置shader/绘制对象时，请务必激活着色器
		shader.use();

		//使用uniform数组时需要
		//for (unsigned int i = 0; i < 100; i++)
		//{
		//	stringstream ss;
		//	string index;
		//	ss << i;
		//	index = ss.str();
		//	shader.setVec2(("offsets[" + index + "]").c_str(), translations[i]);
		//}

		// 创建变换矩阵

		//cube model变换设置
		glBindVertexArray(cubeVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6,100);
		glBindVertexArray(0);


#pragma endregion

		// glfw:交换缓冲区和轮询IO事件(按键按下/释放，鼠标移动等)
		// -----------
		glfwSwapBuffers(window);//重新绘制
		glfwPollEvents();//检查操作系统的返回值信息
	}

	//可选:一旦资源超过使用年限，就重新分配资源:
	// -------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);


	// glfw: terminate，清除之前分配的所有glfw资源。
	// ------------------------------------------------------------------
	glfwDestroyWindow(window);//关闭窗口
	glfwTerminate();//关闭flfw库
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	//设置可绘制的大小
	// ------------------------------
	glViewport(0, 0, width, height); //opgl绘制视口设置，可绘制大小为800 * 600
}

// glfw: 鼠标移动的回调函数
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// 取得与上一帧的偏移差
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // Y轴相反，因为y坐标是从下到上的，鼠标移动的原点在左下放
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// Glfw：鼠标滚轮滑动时的回调函数
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
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

		//如果是透明纹理，使用GL_CLAMP_TO_EDGE来防止半透明边框。
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
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

/// <summary>
/// 从6个单独的纹理面加载立方体贴图纹理
/// +X (right)
/// -X (left)
/// +Y (top)
/// -Y (bottom)
/// +Z (front) 
/// -Z (back)
/// </summary>
/// <param name="faces">6张天空纹理的路径</param>
/// <returns></returns>
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); //天空和需要绑定到 GL_TEXTURE_CUBE_MAP

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			//OpenGL的很多枚举(Enum)一样，它们背后的int值是线性递增的
			//从GL_TEXTURE_CUBE_MAP_POSITIVE_X开始遍历它们，在每个迭代中对枚举值加1，就可以遍历后面相邻的元素
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);//GL_TEXTURE_WRAP_R 为R坐标的环绕方式，跟Z轴一样

	return textureID;
}