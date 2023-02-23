// 2-072-08Shader_Class : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// 着色器类的封装

#include <iostream>


#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <LearnOpenGL/shader.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//屏幕宽高
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


//三角形的顶点数据
float vertices[] = {
    // positions         // colors
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2-08Shader_Class", NULL, NULL);
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

    //创建VAO VBO
    // -----------------------------
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);//glGenVertexArrays用于创建一个VAO
    glGenBuffers(1, &VBO);//glBufferData用于创建一个顶点缓冲对象


    //glbind函数
    //opengl类似一个状态机，当绑定一个VAO时，我们应该绑定和配置对应的VBO.
    //并且应设置VAO槽位的指针指向来用于传递给shader出渲染处理
    //绑定VAO之后，可以解绑VAO供之后使用，当需要用到该VAO时，在进行绑定即可

    //绑定VAO VBO
    // -----------------------------
    glBindVertexArray(VAO);//glBindVertexArray用于绑定一个VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);//glBindBuffer用于绑定一个顶点缓冲对象


    //配置缓冲区内容
    //构建好并绑定VAO与VBO之后,此时VAO，VBO还没有任何数据
    //我们可以通过glVertexAttribPointer与glBufferData来分别配置VAO和VBO
    // -----------------------------
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//glBufferData用于配置VBO
    //顶点位置数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);//glVertexAttribPointer用于配置VAO
    glEnableVertexAttribArray(0);//glEnableVertexAttribArray用来启用VAO的槽位
    //顶点颜色数据
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));//glVertexAttribPointer用于配置VAO
    glEnableVertexAttribArray(1);//glEnableVertexAttribArray用来启用VAO的槽位

    //解绑
    //当配置好VAO和VBO时，可以进行解绑，方便之后使用
    // -----------------------------
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

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
        //绘制图形
        //1：glUseProgram 使用着色器程序
        //2：glBindVertexArray 绑定VAO
        //3: glDrawArrays 绘制图形
        glUseProgram(ourShader.ID);
        glBindVertexArray(VAO);
        /// <summary>
        /// glDrawArrays (GLenum mode, GLint first, GLsizei count)
        /// 该函数根据顶点数组中的坐标数据和指定的模式，进行绘制。
        /// mode:绘制方式:GL_POINTS、GL_LINES、GL_LINE_LOOP、GL_LINE_STRIP、GL_TRIANGLES、GL_TRIANGLE_STRIP、GL_TRIANGLE_FAN。
        /// first，从数组缓存中的哪一位开始绘制，一般为0。
        /// count，数组中顶点的数量。
        /// </summary>
        glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawArrays(GL_LINE_LOOP, 0, 3);

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
