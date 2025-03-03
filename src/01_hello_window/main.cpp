#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;

// make run dir=01_hello_window    dir不要有空格

/// @brief 回调函数当用户改变窗口的大小的时候，视口也应该被调整
/// @param window  窗口
/// @param width_height 窗口改变后的大小
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

/// @brief 检测输入
/// @param window 检测的窗口
void processInput(GLFWwindow *window);

int main()
{
    glfwInit();                                                    // 初始化GLFW ,它主要用于创建/管理窗口，处理用户输入等
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 // 主要版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 // 次要版本  即3.3版本的opengl
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 核心模式

    // 创建glfw窗口   窗口大小，窗口名
    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        cout << "failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // 告诉OpenGL渲染哪个窗口

    // 初始化GLAD  glfwGetProcAddress是OpenGL函数指针地址的函数
    // GLAD 负责查询 OpenGL 驱动程序，并获取正确的 API 函数地址
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // 设置视口尺寸  前两个控制窗口左下角的位置。3，4表示窗口的宽和高
    glViewport(0, 0, 800, 600);

    // 注册窗口大小改变时的回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 循环渲染
    while (!glfwWindowShouldClose(window)) // 窗口是否关闭
    {
        processInput(window); // 检测是否有输入

        // 渲染指令 ....
        // 改变颜色
        glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window); // 交换颜色缓冲
        glfwPollEvents();        // 检测是否有事件
    }

    glfwTerminate(); // 关闭glfw
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // 改变视口大小
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // 输入Esc ，关闭窗口
        glfwSetWindowShouldClose(window, true);
}
