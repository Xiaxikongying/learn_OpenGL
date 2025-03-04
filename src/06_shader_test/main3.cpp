#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <tool/shader.h>
using namespace std;
// 练习3：使用out关键字把顶点位置输出到片段着色器，并将片段的颜色设置为与顶点位置相等

/// @brief 回调函数当用户改变窗口的大小的时候，视口也应该被调整
/// @param window  窗口
/// @param width_height 窗口改变后的大小
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

/// @brief 检测输入
/// @param window 检测的窗口
void processInput(GLFWwindow *window);

string Shader::dirName;

int main(int agrc, char *argv[])
{
    Shader::dirName = argv[1]; // argv[1] = "src/05_shader_class"
    Shader::dirName += "/";

    glfwInit();                                                    // 初始化GLFW ,它主要用于创建/管理窗口，处理用户输入等
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 // 主要版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 // 次要版本  即3.3版本的opengl
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 核心模式

    // 1.1创建glfw窗口   窗口大小，窗口名
    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        cout << "failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // 告诉OpenGL渲染哪个窗口

    // 1.2初始化GLAD  glfwGetProcAddress是OpenGL函数指针地址的函数
    //  GLAD 负责查询 OpenGL 驱动程序，并获取正确的 API 函数地址
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // 1.3设置视口尺寸  前两个控制窗口左下角的位置。3，4表示窗口的宽和高
    // 视口变换，将标准化的点转换为屏幕坐标
    glViewport(0, 0, 800, 600);
    glEnable(GL_PROGRAM_POINT_SIZE); // 启用点大小

    // 注册窗口大小改变时的回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //-----上面是窗口初始化     -----
    //-----下面开始绘制图像的准备-----

    // 2 创建顶点/片段着色器  创建着色器程序
    Shader ourShader("./shader/vertex_test3.glsl", "./shader/fragment_test3.glsl");
    // 3顶点数据
    float vertices[] = {
        // 位置              // 颜色
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // 右下
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 左下
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // 顶部
    };

    // 3创建VBO  VAO  EBO
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);      // 顶点缓冲对象
    glGenVertexArrays(1, &VAO); // 顶点数组对象

    // 3.1 绑定VAO
    // 第一个VAO VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3.4 设置顶点位置属性，   glVertexAttribPointer：设置应该如何解析顶点数据
    // (1): 0表示顶点数据的位置(location = 0)；(2): 3表示当前属性有多少个值； (3):当前属性三个值是float类型；
    // (4): FALSE表示是否希望数据被标准化[-1,1]； (5):步长：距离下一个该属性的值的距离； (6): 数据起始位置的偏移量：0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); // 启用 location = 0 的顶点属性(顶点属性默认禁用)

    // 3.5设置顶点颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // 启用 location = 1 的顶点属性(顶点属性默认禁用)

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 设置线框绘制模式
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 填充模式

    // 4循环渲染
    while (!glfwWindowShouldClose(window)) // 窗口是否关闭
    {
        processInput(window); // 检测是否有输入

        // 渲染指令 ....
        glClearColor(0.2f, 0.3f, 0.5f, 0.9f);
        glClear(GL_COLOR_BUFFER_BIT); // 改变背景颜色

        // 绘制第一个三角形
        ourShader.use();
        glBindVertexArray(VAO);
        // 设置fragment2的uniform
        float timeValue = glfwGetTime(); // 获取时间
        float Value1 = sin(timeValue) / 2;
        float Value2 = cos(timeValue) / 2;

        ourShader.setVec2("xyOffset", Value1, Value2);
        ourShader.setVec3("colorOffset", abs(Value1) * 2, abs(Value2) * 2, abs(Value1) + abs(Value2));

        glDrawArrays(GL_TRIANGLES, 0, 3); // 绘制三角形
        glDrawArrays(GL_POINTS, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window); // 交换颜色缓冲
        glfwPollEvents();        // 检测是否有事件
    }

    // 资源释放
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

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
