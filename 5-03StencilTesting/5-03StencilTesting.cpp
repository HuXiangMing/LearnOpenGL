// 5-03StencilTesting : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
 
/*
模板测试

当片段着色器处理完一个片段之后，模板测试（Stencli Test）开始执行。
和深度测试一样，模板测试也可能会丢弃片段。被保留下来的片段会进入深度测试，他可能会丢弃更多的片段。
模板测试时根据有一个缓冲来进行的，它叫做模板缓冲（Stancli Buffer）。

模板缓冲与模板值
一个模板缓冲中，通常每个模板之时8位的，所以每个像素/片段一共能有256中不同的模板之。
我们可以将这些模板值设置为我们想要的值，然后当某哥片段有某个模板值的时候，我们就可以选择保留或丢弃这个片段。

注意：每个窗口库都需要为你配置一个模板缓冲。GLFW自动做了这件事，所以我们不需要告诉GLFW来创建一个，
但其它的窗口库可能不会默认给你创建一个模板库，所以记得要查看库的文档。

模板缓冲的步骤
模板缓冲操作允许我们在渲染片段时将模板缓冲设定为一个特定的值。通过在渲染时修改模板缓冲的内容，我们写入了模板缓冲。
在同一个渲染（或者接下来的）迭代中，我们可以读取这些值，来决定我们时保留还是丢弃某个片段。
使用模板缓冲的时候可以尽情发挥，大致流程如下：
1.启用模板缓冲的写入。
2.渲染物体，更新模板缓冲的内容。
3.禁用模板缓冲的写入。
4.渲染物体，这次根据模板缓冲的内容丢特定片段。

启用模板缓冲的写入
你可以启用GL_STENCIL_TEST来启用模板测试。glEnable(GL_STENCIL_TEST);
和颜色和深度缓冲一样，你也需要在每次迭代之前清除模板缓冲。glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

和深度测试的glDepthMask函数一样，模板缓冲也有一个类似的函数。
glStencilMask允许我们设置一个位掩码(Bitmask)，它会与将要写入缓冲的模板值进行与(AND)运算。
默认情况下设置的位掩码所有位都为1，不影响输出，但如果我们将它设置为0x00，
写入缓冲的所有模板值最后都会变成0.这与深度测试中的glDepthMask(GL_FALSE)是等价的。
glStencilMask(0xFF); // 每一位写入模板缓冲时都保持原样
glStencilMask(0x00); // 每一位在写入模板缓冲时都会变成0（禁用写入）

模板函数
和深度测试一样，我们对模板缓冲应该通过还是失败，以及它应该如何影响模板缓冲，也是有一定控制的。
一共有两个函数能够用来配置模板测试：glStencilFunc和glStencilOp。
glStencilFunc(GLenum func, GLint ref, GLuint mask)一共包含三个参数：

func：设置模板测试函数(Stencil Test Function)。这个测试函数将会应用到已储存的模板值上和glStencilFunc函数的ref值上。可用的选项有：GL_NEVER、GL_LESS、GL_LEQUAL、GL_GREATER、GL_GEQUAL、GL_EQUAL、GL_NOTEQUAL和GL_ALWAYS。它们的语义和深度缓冲的函数类似。
ref：设置了模板测试的参考值(Reference Value)。模板缓冲的内容将会与这个值进行比较。
mask：设置一个掩码，它将会与参考值和储存的模板值在测试比较它们之前进行与(AND)运算。初始情况下所有位都为1。

glStencilFunc(GL_EQUAL, 1, 0xFF)
这会告诉OpenGL，只要一个片段的模板值等于(GL_EQUAL)参考值1，片段将会通过测试并被绘制，否则会被丢弃。

但是glStencilFunc仅仅描述了OpenGL应该对模板缓冲内容做什么，而不是我们应该如何更新缓冲。这就需要glStencilOp这个函数了。

glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass)一共包含三个选项，我们能够设定每个选项应该采取的行为：

sfail：模板测试失败时采取的行为。
dpfail：模板测试通过，但深度测试失败时采取的行为。
dppass：模板测试和深度测试都通过时采取的行为。

每个选项都可以选用以下的其中一种行为：

行为	描述
GL_KEEP	保持当前储存的模板值
GL_ZERO	将模板值设置为0
GL_REPLACE	将模板值设置为glStencilFunc函数设置的ref值
GL_INCR	如果模板值小于最大值则将模板值加1
GL_INCR_WRAP	与GL_INCR一样，但如果模板值超过了最大值则归零
GL_DECR	如果模板值大于最小值则将模板值减1
GL_DECR_WRAP	与GL_DECR一样，但如果模板值小于0则将其设置为最大值
GL_INVERT	按位翻转当前的模板缓冲值

默认情况下glStencilOp是设置为(GL_KEEP, GL_KEEP, GL_KEEP)的，
所以不论任何测试的结果是如何，模板缓冲都会保留它的值。
默认的行为不会更新模板缓冲，所以如果你想写入模板缓冲的话，你需要至少对其中一个选项设置不同的值。

所以，通过使用glStencilFunc和glStencilOp，我们可以精确地指定更新模板缓冲的时机与行为了，
我们也可以指定什么时候该让模板缓冲通过，即什么时候片段需要被丢弃。

你看到的物体轮廓算法在需要显示选中物体的游戏（想想策略游戏）中非常常见。
这样的算法能够在一个模型类中轻松实现。你可以在模型类中设置一个boolean标记，来设置需不需要绘制边框。
如果你有创造力的话，你也可以使用后期处理滤镜(Filter)，像是高斯模糊(Gaussian Blur)，让边框看起来更自然。

除了物体轮廓之外，模板测试还有很多用途，比如在一个后视镜中绘制纹理，让它能够绘制到镜子形状中，
或者使用一个叫做阴影体积(Shadow Volume)的模板缓冲技术渲染实时阴影。
模板缓冲为我们已经很丰富的OpenGL工具箱又提供了一个很好的工具。

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

float cubeVertices[] = {
	// positions          // texture Coords
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
float planeVertices[] = {
	// positions          // 纹理坐标(注意，我们将这些坐标设置为大于1 (GL_REPEAT作为纹理包装模式)。这将导致地板纹理重复)
	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
	-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
	 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "5-03StencilTesting", NULL, NULL);
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
	glDepthFunc(GL_LESS);//修改深度测试为丢弃深度值大于等于当前深度缓冲值的所有片段。
	glEnable(GL_STENCIL_TEST);//启用模板测试
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);//设置模板函数，在片段模板值不等于1时通过测试
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); //模板测试和深度测试都通过时，将模板值设置为glStencilFunc函数设置的ref值


	// glew加载完成后构建和编译着色器程序
	// ------------------------------------
	Shader shader("vertexSource.vert", "fragmentSource.frag");
	Shader shaderSingleColor("vertexSource.vert", "shaderSingleColorfragmentSource.frag");

#pragma region 处理渲染数据，绘制指令等

	//cube 
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER,cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);


	// plane VAO
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);

#pragma endregion


#pragma region 加载贴图

	unsigned int cubeTexture = loadTexture(FileSystem::getPath("resources/textures/marble.jpg").c_str());
	unsigned int floorTexture = loadTexture(FileSystem::getPath("resources/textures/metal.png").c_str());

#pragma endregion


	shader.use();
	shader.setInt("texture1", 0);

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // 和颜色和深度缓冲一样，也需要在每次迭代之前清除模板缓冲

		//在设置shader/绘制对象时，请务必激活着色器
		shader.use();		

		// 创建变换矩阵
		glm::mat4 model = glm::mat4(1.0f);//模型矩阵
		glm::mat4 view = glm::mat4(1.0f);//视图矩阵
		glm::mat4 projection = glm::mat4(1.0f);//投影矩阵

		//视图/投影 变换
		view = camera.My_GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);//fov = 45, 屏幕比例：800/600,近裁剪面：0.1，远裁剪面：100
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		shaderSingleColor.use();
		shaderSingleColor.setMat4("view", view);
		shaderSingleColor.setMat4("projection", projection);

		shader.use();

		/*我们只想给那两个箱子加上边框，所以我们让地板不参与这个过程。
		我们希望首先绘制地板，再绘制两个箱子（并写入模板缓冲）
		之后绘制放大的箱子（并丢弃覆盖了之前绘制的箱子片段的那些片段）。
		我们将其掩码设置为0x00，以不写入模板缓冲区。*/
		glStencilMask(0x00);
		// floor model变换设置
		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		model = glm::mat4(1.0f);
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		//对箱子中所有绘制的片段，将模板值更新为1
		// -----------
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // 所有的片段都应该更新模板缓冲的模板值为1
		glStencilMask(0xFF); // 启用模板缓冲写入
		//cube model变换设置
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//cube2 model变换设置
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//现在模板缓冲在箱子被绘制的地方都更新为1了，我们将要绘制放大的箱子，并禁用模板缓冲的写入：
		// -----------
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // 在模板测试时的片段模板值不等于1时通过
		glStencilMask(0x00); // 禁止模板缓冲的写入
		glDisable(GL_DEPTH_TEST);//禁用深度测试，让放大的箱子，即边框，不会被地板所覆盖，并且在与没有放大的正常箱子覆盖时，由于做了模板测试，与正方体覆盖的地方会被舍弃掉
		//接下来绘制放大版本的箱子
		shaderSingleColor.use();
		float scale = 1.1f;
		glBindVertexArray(cubeVAO);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		model = glm::scale(model, glm::vec3(scale, scale, scale));
		shaderSingleColor.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(scale, scale, scale));
		shaderSingleColor.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//重置，确保不会影响到下一次渲染
		glBindVertexArray(0);
		glStencilMask(0xFF);//启用模板缓冲的写入
		glStencilFunc(GL_ALWAYS, 0, 0xFF);// 所有的片段都应该更新模板缓冲模板值为0
		glEnable(GL_DEPTH_TEST);//启用深度缓冲
#pragma endregion

		// glfw:交换缓冲区和轮询IO事件(按键按下/释放，鼠标移动等)
		// -----------
		glfwSwapBuffers(window);//重新绘制
		glfwPollEvents();//检查操作系统的返回值信息
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &planeVBO);

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