// 2-09Textures : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// SOIL的图像库使用，以及纹理贴图的与纹理坐标的应用
// 
// 关于为什么不用传给uniform传值就可以显示图片
// 你可能会奇怪为什么sampler2D变量是个uniform，我们却不用glUniform给它赋值。
// 使用glUniform1i，我们可以给纹理采样器分配一个位置值，这样的话我们能够在一个片段着色器中设置多个纹理。一个纹理的位置值通常称为一个纹理单元(Texture Unit)。
// 一个纹理的默认纹理单元是0，它是默认的激活纹理单元

// 纹理单元的主要目的是让我们在着色器中可以使用多于一个的纹理。
// 通过把纹理单元赋值给采样器，我们可以一次绑定多个纹理，只要我们首先激活对应的纹理单元。
// 就像glBindTexture一样，我们可以使用glActiveTexture激活纹理单元，传入我们需要使用的纹理单元：

// glTexParameter*函数对单独的一个坐标轴设置（s、t（如果是使用3D纹理那么还有一个r）它们和x、y、z是等价的）
#include <iostream>

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <LearnOpenGL/shader.h>
#include <LearnOpenGL/filesystem.h>

#define STB_IMAGE_IMPLEMENTATION //使用stb_image.h 请先定义宏STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//屏幕宽高
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


//三角形的顶点数据
float vertices[] = {
	// positions          // colors           // texture coords
	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
};

unsigned int indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
};

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2-09Textures", NULL, NULL);
	if (window == NULL)
	{
		printf("窗口创建失败");
		glfwTerminate();//终止掉glfw
		return -1;
	}

	glfwMakeContextCurrent(window);//设置为glfw的主窗口 
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// glew:加载所有OpenGL函数指针
	// -----------------------------
	glewExperimental = true;//GLEW在管理OpenGL的函数指针时更多地使用现代化的技术。
	if (glewInit() != GLEW_OK)
	{
		printf("glew初始化失败");
		return -1;
	}

	// glew加载完成后构建和编译着色器程序
	// ------------------------------------
	Shader ourShader("vertexSource.vert", "fragmentSource.frag");

#pragma region 处理渲染数据，绘制指令等

	//VAO 
	//顶点数组对象，VAO中有许多特性槽位的指针，每个特性槽位都可以与VBO中的顶点数据进行绑定
	//通过glVertexAttribPointer设置顶点属性配置。

	//VBO
	//顶点缓冲对象，opengl通过VBO来管理内存，VBO通常会在显卡储存大量的顶点数据（这些数据来自CPU）
	//这些顶点数据包括了顶点位置，发现，顶点颜色，纹理坐标等等
	//opengl的缓冲对象类型有许多，顶点缓冲对象的类型为GL_ARRAY_BUFFER

	//EBO
	//索引缓冲对象，它存储 OpenGL 用来决定要绘制哪些顶点的索引。

	//创建VAO VBO EBO
	// -----------------------------
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);//glGenVertexArrays用于创建一个VAO
	glGenBuffers(1, &VBO);//glBufferData用于创建一个顶点缓冲对象
	glGenBuffers(1, &EBO);//glBufferData用于创建一个顶点缓冲对象

	//glbind函数
	//opengl类似一个状态机，当绑定一个VAO时，我们应该绑定和配置对应的VBO.
	//并且应设置VAO槽位的指针指向来用于传递给shader出渲染处理
	//绑定VAO之后，可以解绑VAO供之后使用，当需要用到该VAO时，在进行绑定即可

	//绑定VAO VBO EBO
	// -----------------------------
	glBindVertexArray(VAO);//glBindVertexArray用于绑定一个VAO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//glBindBuffer用于绑定一个顶点缓冲对象
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//配置缓冲区内容
	//构建好并绑定VAO与VBO之后,此时VAO，VBO还没有任何数据
	//我们可以通过glVertexAttribPointer与glBufferData来分别配置VAO和VBO
	// -----------------------------
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//glBufferData用于配置VBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);//glBufferData用于配置EBO
	//顶点位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);//glVertexAttribPointer用于配置VAO
	glEnableVertexAttribArray(0);//glEnableVertexAttribArray用来启用VAO的槽位
	//顶点颜色
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//顶点的纹理坐标
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//解绑
	//当配置好VAO和VBO时，可以进行解绑，方便之后使用
	// -----------------------------
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

#pragma endregion


#pragma region 加载贴图

	//1:glGenTextures 创建纹理单元
	//2:glBindTexture 绑定纹理单元，使得接下来opengl的操作都会针对于该纹理单元
	//  纹理单元GL_TEXTURE0默认总是被激活，如果之前没有激活纹理单元默认glBindTexture 会默认绑定到GL_TEXTURE0
	//3：glTexParameteri  为当前绑定的纹理对象设置环绕、过滤方式
	//4：加载图像，创建纹理和生成mipmaps

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//设置纹理环绕方式
	//  GL_REPEAT	对纹理的默认行为。重复纹理图像。 
	//  GL_MIRRORED_REPEAT	和GL_REPEAT一样，但每次重复图片是镜像放置的。
	//  GL_CLAMP_TO_EDGE	纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。
	//  GL_CLAMP_TO_BORDER	超出的坐标为用户指定的边缘颜色。
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	//设置纹理过滤方式 纹理过滤有很多个选项 最重要的两种：GL_NEAREST邻近过滤和GL_LINEAR线性过滤
	//  GL_NEAREST_MIPMAP_NEAREST	使用最邻近的多级渐远纹理来匹配像素大小，并使用邻近插值进行纹理采样
	//  GL_LINEAR_MIPMAP_NEAREST	使用最邻近的多级渐远纹理级别，并使用线性插值进行采样
	//  GL_NEAREST_MIPMAP_LINEAR	在两个最匹配像素大小的多级渐远纹理之间进行线性插值，使用邻近插值进行采样
	//  GL_LINEAR_MIPMAP_LINEAR	    在两个邻近的多级渐远纹理之间使用线性插值，并使用线性插值进行采样
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//邻近过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//线性过滤

	int width, height, nrChannels;
	//SOIL的图像库stbi_load函数 用来加载一张图片
	unsigned char* data = stbi_load(FileSystem::getPath("Resources/textures/container.jpg").c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		//glTexImage2D可以生成一张纹理
		//第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。
		//第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。
		//第三个参数告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值。
		//第四个和第五个参数设置最终的纹理的宽度和高度。我们之前加载图像的时候储存了它们，所以我们使用对应的变量。
		//下个参数应该总是被设为0（历史遗留的问题）。
		//第七第八个参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为char(byte)数组，我们将会传入对应值。
		//最后一个参数是真正的图像数据。
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap为当前绑定的纹理自动生成所有需要的多级渐远纹理
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	//释放图像的内存
	stbi_image_free(data);

#pragma endregion

	// 渲染循环
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// 处理输入
		// -----------
		processInput(window);

		// 处理渲染
		// -----------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

#pragma region Draw
		glBindTexture(GL_TEXTURE_2D, texture);

		glUseProgram(ourShader.ID);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


#pragma endregion

		// glfw:交换缓冲区和轮询IO事件(按键按下/释放，鼠标移动等)
		// -----------
		glfwSwapBuffers(window);//重新绘制
		glfwPollEvents();//检查操作系统的返回值信息
	}


	//可选:一旦资源超过使用年限，就重新分配资源:
	// ------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(ourShader.ID);


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
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

	//设置可绘制的大小
	// ------------------------------
	glViewport(0, 0, width, height); //opgl绘制视口设置，可绘制大小为800 * 600
}
