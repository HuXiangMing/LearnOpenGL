// 2-01GLEW.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>


#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//屏幕宽高
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "1-02GLEW_Window", NULL, NULL);
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
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

    //设置可绘制的大小
    // ------------------------------
    glViewport(0, 0, width, height); //opgl绘制视口设置，可绘制大小为800 * 600
}
