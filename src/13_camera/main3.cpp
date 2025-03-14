#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>
#include <tool/shader.h>
#include <tool/gui.h>
#include <tool/camera.h>

#include <geometry/BoxGeometry.h>
#include <math.h>
#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_calback(GLFWwindow *window, int button, int action, int mods);
void scroll_callback(GLFWwindow *window, double x, double y);

string Shader::dirName;

int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 900;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float lastX = SCREEN_WIDTH / 2.0f; // 鼠标上一帧的位置
float lastY = SCREEN_HEIGHT / 2.0f;

// 初始化相机
Camera camera(glm::vec3(0.0, 1.0, 5.0)); // 相机位置

int main(int agrc, char *argv[])
{
    Shader::dirName = argv[1]; // ex: argv[1] = "src/05_shader_class"
    Shader::dirName += "/";

    glfwInit();                                                    // 初始化GLFW ,它主要用于创建/管理窗口，处理用户输入等
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 // 主要版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 // 次要版本  即3.3版本的opengl
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 核心模式

    // 建glfw窗口   窗口大小，窗口名
    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    //--------------imgui设置--------------
    // 创建GUI上下文
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // 设置imgui主题颜色
    // ImGui::StyleColorsClassic(); // 经典皮肤
    ImGui::StyleColorsDark(); // 暗色

    // 设置平台
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char *glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);
    //--------------imgui设置完成--------------

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);                                // 启用颜色混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 设定混合模式，实现 基于 alpha 透明度的颜色混合
    glEnable(GL_DEPTH_TEST);                           // 开启深度测试

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetMouseButtonCallback(window, mouse_button_calback);
    glfwSetScrollCallback(window, scroll_callback);

    // 生成着色器
    Shader ourShader("./shader/vertex.glsl", "./shader/fragment.glsl");
    // 生成一个平面几何图形
    BoxGeometry boxGeometry(1.0f, 1.0f, 1.0f);

    // 生成纹理
    {
        unsigned int texture1, texture2; // 纹理ID
        // 采入第一个纹理
        glGenTextures(1, &texture1);            // 生成纹理id
        glBindTexture(GL_TEXTURE_2D, texture1); // 绑定纹理，让之后任何的纹理指令都可以配置当前绑定的纹理

        // 设置环绕和过滤方式
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     // x轴的过滤方式   重复
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     // Y轴
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 图像缩小   线性过滤
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 图像放大

        // 图像y轴翻转  放任意位置
        stbi_set_flip_vertically_on_load(true);

        // 加载图片
        int width, height, nrChannels; // 颜色通道的个数(3通道rgb   4通道rgba)
        unsigned char *data = stbi_load("./static/texture/container.jpg", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D); // 使用opengl生成多级渐远纹理（Mipmap）
        }
        stbi_image_free(data);

        // 采入第二个纹理
        glGenTextures(1, &texture2);                                      // 生成纹理id
        glBindTexture(GL_TEXTURE_2D, texture2);                           // 绑定纹理，让之后任何的纹理指令都可以配置当前绑定的纹理
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     // x轴的环绕方式
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     // Y轴的环绕方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 图像缩小的过滤模式 ：线性过滤  还有一种是GL_NEAREST  就近过滤
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 图像放大
        data = stbi_load("./static/texture/awesomeface.png", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D); // 使用opengl生成多级渐远纹理（Mipmap）
        }
        stbi_image_free(data);

        ourShader.use();
        ourShader.setInt("Texture1", 0);
        ourShader.setInt("Texture2", 1);

        glActiveTexture(GL_TEXTURE0);           // 激活纹理0  默认激活0，如果只有一个纹理单元的话
        glBindTexture(GL_TEXTURE_2D, texture1); // 让纹理0绑定texture1
        glActiveTexture(GL_TEXTURE1);           // 激活纹理1
        glBindTexture(GL_TEXTURE_2D, texture2); // 让纹理1绑定texture2
    }

    float value = 0;
    float aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
    glm::vec3 clear_color = glm::vec3(0.2f, 0.3f, 0.5f); // 背景颜色
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 循环渲染
    while (!glfwWindowShouldClose(window)) // 窗口是否关闭
    {
        processInput(window); // 检测是否有输入
        // 创建imgui窗口
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 设置渲染内容
        ImGui::Begin("ImGui");
        // 显示帧率
        ImGui::Text("%.3f ms-fram (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        // 显示滑动条
        ImGui::SliderFloat("fov", &camera.Zoom, 1.0f, 60.0f);
        ImGui::SliderFloat("sensitivity", &camera.MouseSensitivity, 0.01f, 0.5f);
        ImGui::SliderFloat3("view", (float *)&camera.Position, -50.0f, 50.0f);
        ImGui::End();

        ImGui::Begin("ImGui2");
        ImGui::ColorEdit3("clear color", (float *)&clear_color);
        ImGui::End();

        // 渲染指令 ....
        glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
        // 清空颜色缓存  清空深度缓存
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        value = glfwGetTime();
        deltaTime = value - lastFrame; // 计算与上一帧之间的时间差
        lastFrame = value;             // 记录当前帧的时间

        // model矩阵
        glm::mat4 model = glm::mat4(1.0f);
        ourShader.setMat4("model", model);
        // view矩阵
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);
        // projection矩阵
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), aspect, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        glBindVertexArray(boxGeometry.VAO);
        glDrawElements(GL_TRIANGLES, boxGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        // 开始渲染imgui  写在图形后面，才不会被图形遮挡
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window); // 交换颜色缓冲
        glfwPollEvents();        // 检测是否有事件
    }
    boxGeometry.dispose();
    glfwTerminate(); // 关闭glfw
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // 改变视口大小
    glViewport(0, 0, width, height);
}

bool isTabPressed = false; // 用来标记Tab键是否按下过
int last_fov = 0;
bool isCursorLocked = true;
/// @brief 按键回调函数
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
    {
        // ALT 按下：显示鼠标
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        isCursorLocked = false;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE && !isCursorLocked)
    {
        // ALT 松开：隐藏鼠标
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        isCursorLocked = true;
    }

    Camera_Movement key = NONE;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        key = ACCELERATE;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(key | FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(key | BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(key | LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(key | RIGHT, deltaTime);
    }

    // 放大视角
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !isTabPressed)
    {
        last_fov = camera.Zoom;
        camera.Zoom = 10.0f;
        isTabPressed = true; // 标记 Tab 键已按下
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE && isTabPressed)
    {
        camera.Zoom = last_fov;
        isTabPressed = false; // 标记 Tab 键已抬起
    }
}

/// @brief 鼠标移动回调函数
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    if (isCursorLocked)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

/// @brief 鼠标滚轮回调函数
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.Zoom -= (float)yoffset;
    if (camera.Zoom < 1.0f)
        camera.Zoom = 1.0f;
    if (camera.Zoom > 60.0f)
        camera.Zoom = 60.0f;
    return;
}