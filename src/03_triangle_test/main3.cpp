#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;

// 练习3： 创建两个着色器程序，让两个三角形颜色不同

/// @brief 回调函数当用户改变窗口的大小的时候，视口也应该被调整
/// @param window  窗口
/// @param width_height 窗口改变后的大小
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

/// @brief 检测输入
/// @param window 检测的窗口
void processInput(GLFWwindow *window);

// 顶点着色器  这里姑且先使用字符串形式写
const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
                                 "gl_PointSize = 10.0f;\n"
                                 "}\n\0";
// 片段着色器
const char *fragmentShaderSource1 = "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "void main()\n"
                                    "{\n"
                                    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                    "}\n\0";

// 片段着色器
const char *fragmentShaderSource2 = "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "void main()\n"
                                    "{\n"
                                    "   FragColor = vec4(0.2f, 0.7f, 0.8f, 1.0f);\n"
                                    "}\n\0";

int main()
{
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

    // 2.1创建一个顶点着色器
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER); // 返回顶点着色器的索引（id）
    // 附加着色器源码
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // 参数为：着色器id，字符串数量，着色器代码，
    // 编译着色器
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    // 可以用下面代码检测着色器创建是否成功
    {
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                 << infoLog << endl;
        }
    }

    // 2.2 创建片段着色器
    // 总体与顶点着色器类型
    unsigned int fragmentShaders[2];
    fragmentShaders[0] = glCreateShader(GL_FRAGMENT_SHADER); // 返回片段着色器的索引（id）
    // 附加着色器源码
    glShaderSource(fragmentShaders[0], 1, &fragmentShaderSource1, NULL);
    // 编译着色器
    glCompileShader(fragmentShaders[0]);
    // 可以用下面代码检测着色器创建是否成功
    {
        glGetShaderiv(fragmentShaders[0], GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShaders[0], 512, NULL, infoLog);
            cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                 << infoLog << endl;
        }
    }

    fragmentShaders[1] = glCreateShader(GL_FRAGMENT_SHADER); // 返回片段着色器的索引（id）
    // 附加着色器源码
    glShaderSource(fragmentShaders[1], 1, &fragmentShaderSource2, NULL);
    // 编译着色器
    glCompileShader(fragmentShaders[1]);
    // 可以用下面代码检测着色器创建是否成功
    {
        glGetShaderiv(fragmentShaders[1], GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShaders[1], 512, NULL, infoLog);
            cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                 << infoLog << endl;
        }
    }

    // 2.3着色器程序 将多个着色器合并
    unsigned int shaderPrograms[2];
    shaderPrograms[0] = glCreateProgram(); // 创建着色器程序
    // 合并顶点、片段着色器
    glAttachShader(shaderPrograms[0], vertexShader);
    glAttachShader(shaderPrograms[0], fragmentShaders[0]);
    glLinkProgram(shaderPrograms[0]);
    // 可以用下面代码检测着色器创建是否成功
    {
        glGetProgramiv(shaderPrograms[0], GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shaderPrograms[0], 512, NULL, infoLog);
            cout << "ERROR::SHADER::shaderProgram::COMPILATION_FAILED\n"
                 << infoLog << endl;
        }
    }

    shaderPrograms[1] = glCreateProgram(); // 创建着色器程序
    // 合并顶点、片段着色器
    glAttachShader(shaderPrograms[1], vertexShader);
    glAttachShader(shaderPrograms[1], fragmentShaders[1]);
    glLinkProgram(shaderPrograms[1]);
    // 可以用下面代码检测着色器创建是否成功
    {
        glGetProgramiv(shaderPrograms[1], GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shaderPrograms[1], 512, NULL, infoLog);
            cout << "ERROR::SHADER::shaderProgram::COMPILATION_FAILED\n"
                 << infoLog << endl;
        }
    }

    // 在glLinkProgram函数调用之后，每个着色器调用和渲染调用都会使用shaderProgram
    // 所以vertexShader和fragmentShader就可以删除了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShaders[0]);
    glDeleteShader(fragmentShaders[1]);

    // 3顶点数据
    float vertices[] = {
        -0.9f, -0.5f, 0.0f, // left
        -0.0f, -0.5f, 0.0f, // right
        -0.45f, 0.5f, 0.0f, // top

        0.0f, -0.5f, 0.0f, // left
        0.9f, -0.5f, 0.0f, // right
        0.45f, 0.5f, 0.0f  // top
    };

    // 3创建VBO  VAO  EBO
    unsigned int VBOS[2], VAOS[2];
    glGenBuffers(2, VBOS);      // 顶点缓冲对象
    glGenVertexArrays(2, VAOS); // 顶点数组对象

    // 3.1 绑定VAO
    // 第一个VAO VBO
    glBindVertexArray(VAOS[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOS[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3.4 设置顶点属性，glVertexAttribPointer：设置应该如何解析顶点数据
    // (1): 0表示顶点数据的位置(location = 0)；(2): 3表示顶点数据有三个值vec3； (3):顶点的三个值是float；
    // (4): FALSE表示是否希望数据被标准化[-1,1]； (5):步长：顶点属性所占的大小； (6): 数据起始位置的偏移量：0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); // 启用 location = 0 的顶点属性(顶点属性默认禁用)

    // 第二个VAO VBO
    glBindVertexArray(VAOS[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOS[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(9 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 设置线框绘制模式
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 填充模式

    // 4循环渲染
    while (!glfwWindowShouldClose(window)) // 窗口是否关闭
    {
        processInput(window); // 检测是否有输入

        // 渲染指令 ....
        glClearColor(0.2f, 0.3f, 0.5f, 0.9f);
        glClear(GL_COLOR_BUFFER_BIT); // 改变背景颜色

        glUseProgram(shaderPrograms[0]);
        glBindVertexArray(VAOS[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3); // 绘制三角形
        glDrawArrays(GL_POINTS, 0, 3);

        glUseProgram(shaderPrograms[1]);
        glBindVertexArray(VAOS[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3); // 绘制三角形
        glDrawArrays(GL_POINTS, 0, 3);

        glfwSwapBuffers(window); // 交换颜色缓冲
        glfwPollEvents();        // 检测是否有事件
    }

    // 资源释放
    glDeleteVertexArrays(2, VAOS);
    glDeleteBuffers(2, VBOS);
    glDeleteProgram(shaderPrograms[0]);
    glDeleteProgram(shaderPrograms[1]);

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
