#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>
#include <tool/shader.h>
#include <tool/gui.h>
#include <tool/camera.h>

#include <geometry/BoxGeometry.h>
#include <geometry/SphereGeometry.h>
#include <math.h>
#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_calback(GLFWwindow *window, int button, int action, int mods);
void scroll_callback(GLFWwindow *window, double x, double y);

// 加载贴图
unsigned int loadTexture(char const *path);

string Shader::dirName;

int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 900;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float lastX = SCREEN_WIDTH / 2.0f; // 鼠标上一帧的位置
float lastY = SCREEN_HEIGHT / 2.0f;

// 初始化相机
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f)); // 相机位置

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
    ImGui::StyleColorsDark(); // 暗色   //ImGui::StyleColorsClassic();//经典皮肤

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

    // 注册回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // 生成着色器
    Shader ourShader("./shader/direction/dir_vertex.glsl", "./shader/direction/dir_fragment.glsl");
    Shader lightShader("./shader/light_vert.glsl", "./shader/light_frag.glsl");

    BoxGeometry boxGeometry(1.0f, 1.0f, 1.0f);
    // SphereGeometry sphereGeometry(0.75f);
    SphereGeometry light(0.2f); // 光源

    float value = 0; // 时间值
    float aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
    glm::vec3 clearColor = glm::vec3(0.2f, 0.3f, 0.5f);  // 背景颜色
    glm::vec3 cubeColor = glm::vec3(1.0f, 1.0f, 1.0f);   // 正方形颜色
    glm::vec3 sphereColor = glm::vec3(0.8f, 0.2f, 0.5f); // 球颜色

    glm::vec3 lightPos = glm::vec3(2.0f, 1.0f, 3.0f);   // 光源位置
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // 光源颜色

    // 加载漫反射 镜面反射贴图
    unsigned int diffuseMap = loadTexture("./static/texture/container2.png");
    unsigned int specularMap = loadTexture("./static/texture/container2_specular.png");

    // 激活使用两张贴图
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    ourShader.use();
    // 传递材质属性
    ourShader.setInt("material.diffuse", 0);
    ourShader.setInt("material.specular", 1);
    ourShader.setFloat("material.shininess", 32.0f);

    // 设置光照属性
    ourShader.setVec3("light.ambient", 0.5f, 0.5f, 0.5f);
    ourShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
    ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    ourShader.setVec3("light.direction", lightPos);

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

    // 循环渲染
    while (!glfwWindowShouldClose(window)) // 窗口是否关闭
    {
        processInput(window); // 检测是否有输入

        {
            // 创建imgui窗口
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // 设置渲染内容
            ImGui::Begin("Camera");
            // 显示帧率
            ImGui::Text("%.3f ms-fram (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            // 显示滑动条
            ImGui::SliderFloat("fov", &camera.Zoom, 1.0f, 60.0f);
            ImGui::SliderFloat("mouseSensitivity", &camera.MouseSensitivity, 0.01f, 0.5f);
            ImGui::SliderFloat("view.x", &camera.Position.x, -30.0f, 30.0f); // 范围
            ImGui::SliderFloat("view.y", &camera.Position.y, -30.0f, 30.0f);
            ImGui::SliderFloat("view.z", &camera.Position.z, -30.0f, 30.0f);
            ImGui::End();

            ImGui::Begin("Lighting");
            ImGui::ColorEdit3("cubeColor", (float *)&cubeColor);
            ImGui::ColorEdit3("sphereColor", (float *)&sphereColor);
            ImGui::ColorEdit3("lightColor", (float *)&lightColor);
            ImGui::End();
        }

        // 渲染指令 ....
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        // 清空颜色缓存  清空深度缓存
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        value = glfwGetTime();
        deltaTime = value - lastFrame; // 计算与上一帧之间的时间差
        lastFrame = value;             // 记录当前帧的时间

        // model矩阵
        glm::mat4 model = glm::mat4(1.0f);
        // view矩阵
        glm::mat4 view = camera.GetViewMatrix();
        // projection矩阵
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), aspect, 0.1f, 100.0f);

        // lightPos.x = sin(value) * 2;
        // lightPos.y = cos(value) * 2;

        // 物体着色器
        ourShader.use();
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setVec3("objectColor", cubeColor);

        // 绘制立方体
        model = glm::mat4(1.0f);
        glBindVertexArray(boxGeometry.VAO);
        for (int i = 0; i < 10; ++i)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            if (i % 3 == 0)
                model = glm::rotate(model, value * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
            ourShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, boxGeometry.indices.size(), GL_UNSIGNED_INT, 0);
        }

        // // 绘制球体
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        // ourShader.setMat4("model", model);
        // ourShader.setVec3("objectColor", sphereColor);
        // glBindVertexArray(sphereGeometry.VAO);
        // glDrawElements(GL_TRIANGLES, sphereGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        // 光源着色器
        lightShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        lightShader.setMat4("model", model);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightShader.setVec3("lightColor", lightColor);
        glBindVertexArray(light.VAO);
        glDrawElements(GL_TRIANGLES, light.indices.size(), GL_UNSIGNED_INT, 0);

        // 开始渲染imgui  写在图形后面，才不会被图形遮挡
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window); // 交换颜色缓冲
        glfwPollEvents();        // 检测是否有事件
    }
    boxGeometry.dispose();
    // sphereGeometry.dispose();
    light.dispose();
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

/// @brief 按键回调函数
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
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

// 加载纹理贴图
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
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