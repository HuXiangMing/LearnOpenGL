// 5-18Geometry_Shader_Exploding : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

/*
几何着色器
应用：通过几何着色器来修改原有的渲染方式，爆破效果

爆破物体
当我们说爆破一个物体时，我们并不是指要将宝贵的顶点集给炸掉，我们是要将每个三角形沿着法向量的方向移动一小段时间。
效果就是，整个物体看起来像是沿着每个三角形的法线向量爆炸一样。

这样的几何着色器效果的一个好处就是，无论物体有多复杂，它都能够应用上去。

求得一个三角形面的法向量
因为我们想要沿着三角形的法向量位移每个顶点，我们首先需要计算这个法向量。
我们所要做的是计算垂直于三角形表面的向量，仅使用我们能够访问的3个顶点。
我们使用叉乘来获取垂直于其它两个向量的一个向量。如果我们能够获取两个平行于三角形表面的向量a和b，我们就能够对这两个向量进行叉乘来获取法向量了。

就像这样
vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}

这里我们使用减法获取了两个平行于三角形表面的向量a和b。
因为两个向量相减能够得到这两个向量之间的差值，并且三个点都位于三角平面上，对任意两个向量相减都能够得到一个平行于平面的向量。
注意，如果我们交换了cross函数中a和b的位置，我们会得到一个指向相反方向的法向量——这里的顺序很重要！

爆炸函数explode
它使用法向量和顶点位置向量作为参数。这个函数会返回一个新的向量，它是位置向量沿着法线向量进行位移之后的结果：
vec4 explode(vec4 position, vec3 normal)
{
	float magnitude = 2.0;
	vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
	return position + vec4(direction, 0.0);
}
函数本身应该不是非常复杂。sin函数接收一个time参数，它根据时间返回一个-1.0到1.0之间的值。
因为我们不想让物体向内爆炸(Implode)，我们将sin值变换到了[0, 1]的范围内。
最终的结果会乘以normal向量，并且最终的direction向量会被加到位置向量上。

3D模型看起来随着时间不断在爆破它的顶点，在这之后又回到正常状态。
虽然这并不是非常有用，它的确向你展示了几何着色器更高级的用法。
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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "5-18Geometry_Shader_Exploding", NULL, NULL);
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
	//Shader shader("vertexSource.vert", "fragmentSource.frag");
	Shader shader("vertexSource.vert", "fragmentSource.frag", "geometrySource.geom");


	// 加载模型
	// ------------------------------------
	Model ourModel(FileSystem::getPath("resources/objects/nanosuit/nanosuit.obj").c_str());

#pragma region 处理渲染数据，绘制指令等	

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

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();;
		glm::mat4 model = glm::mat4(1.0f);


		//在设置shader/绘制对象时，请务必激活着色器
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);
		shader.setFloat("time", static_cast<float>(glfwGetTime()));

		ourModel.Draw(shader);

#pragma endregion

		// glfw:交换缓冲区和轮询IO事件(按键按下/释放，鼠标移动等)
		// -----------
		glfwSwapBuffers(window);//重新绘制
		glfwPollEvents();//检查操作系统的返回值信息
	}


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