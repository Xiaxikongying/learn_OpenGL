#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

#include <tool/shader.h>
#include <tool/camera.h>
#include <geometry/BoxGeometry.h>
#include <geometry/PlaneGeometry.h>
#include <geometry/SphereGeometry.h>

#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>
#include <tool/gui.h>
#include <map>
using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
unsigned int loadTexture(char const *path);

void drawMesh(BufferGeometry geometry);
void drawLightObject(Shader shader, BufferGeometry geometry, glm::vec3 position);

std::string Shader::dirName;
int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 900;
float deltaTime = 0.0f;
float lastTime = 0.0f;
float lastX = SCREEN_WIDTH / 2.0f; // 鼠标上一帧的位置
float lastY = SCREEN_HEIGHT / 2.0f;

Camera camera(glm::vec3(0.0, 1.0, 6.0));

int main(int argc, char *argv[])
{
    Shader::dirName = argv[1]; // ex: argv[1] = "src/05_shader_class"
    Shader::dirName += "/";

    glfwInit();
    const char *glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 窗口对象
    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    {
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        ImGui::StyleColorsDark(); // 暗色
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        const char *glsl_version = "#version 330";
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    // 设置视口
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    // 混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 源因子  目标因子
    // 深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // 鼠标键盘事件
    // 1.注册窗口变化监听
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Shader sceneShader("./shader/scene_vert.glsl", "./shader/scene_frag.glsl");
    Shader blurShader("./shader/blur_scene_vert.glsl", "./shader/blur_scene_frag.glsl");            // 模糊
    Shader finalShader("./shader/bloom_final_vert.glsl", "./shader/bloom_final_frag.glsl");         // bloom
    Shader lightObjectShader("./shader/light_object_vert.glsl", "./shader/light_object_frag.glsl"); // 光源

    PlaneGeometry groundGeometry(10.0, 10.0);           // 地面
    PlaneGeometry blendGeometry(1.0, 1.0);              // 透明物体
    BoxGeometry boxGeometry(1.0, 1.0, 1.0);             // 盒子
    SphereGeometry pointLightGeometry(0.2, 50.0, 50.0); // 点光源位置显示
    PlaneGeometry quadGeometry(2.0, 2.0);               // hdr输出平面

    unsigned int woodMap = loadTexture("./static/texture/wood.png");                         // 地面贴图
    unsigned int brickMap = loadTexture("./static/texture/brick_diffuse.jpg");               // 砖块贴图
    unsigned int blendMap = loadTexture("./static/texture/blending_transparent_window.png"); // 透明物体贴图

    glm::vec4 clear_color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    glm::vec3 lightPosition = glm::vec3(1.0, 2.5, 2.0); // 光照位置

    // 由于颜色只能在[0,1]，所有创建一个使用GL_RGBA16F作为颜色大小的帧缓冲
    // 创建hdr帧缓冲
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO); // 绑定hdr帧缓冲
    // 创建两个颜色缓冲区，一个用于存储 HDR 场景颜色，一个用于用于提取亮部区域以进行泛光
    // 一个帧缓冲绑定两个颜色缓冲取---->绘制时的片段着色器需要输出两个颜色--->两个FragColor
    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // 避免模糊过滤器重复采样
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    // 深度缓冲
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    // 为hdr帧缓冲区 设置颜色附件  否则默认只会渲染到GL_COLOR_ATTACHMENT0。
    unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // 解绑hdr

    // 创建两个额外的帧缓冲，用于高斯模糊处理
    unsigned int pingpongFBO[2];
    glGenFramebuffers(2, pingpongFBO);
    // 两个模糊颜色缓冲
    unsigned int pingpongColorbuffers[2];
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]); // 将颜色缓冲绑定到帧缓冲上
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }

    blurShader.use(); // 模糊
    blurShader.setInt("image", 0);

    finalShader.use(); // 最终效果
    finalShader.setInt("scene", 0);
    finalShader.setInt("bloomBlur", 1);

    sceneShader.use();
    sceneShader.setInt("Texture", 0);
    // 设置平行光光照属性
    sceneShader.setVec3("directionLight.ambient", 0.01f, 0.01f, 0.01f);
    sceneShader.setVec3("directionLight.diffuse", 0.2f, 0.2f, 0.2f); // 将光照调暗了一些以搭配场景
    sceneShader.setVec3("directionLight.specular", 1.0f, 1.0f, 1.0f);
    // 设置衰减
    sceneShader.setFloat("light.constant", 1.0f);
    sceneShader.setFloat("light.linear", 0.09f);
    sceneShader.setFloat("light.quadratic", 0.032f);

    // 点光源的位置
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 1.0f, 1.5f),
        glm::vec3(2.3f, 3.0f, -4.0f),
        glm::vec3(-4.0f, 2.0f, 1.0f),
        glm::vec3(1.4f, 2.0f, 0.3f)};
    // 点光源颜色
    glm::vec3 pointLightColors[] = {
        glm::vec3(2.0f, 1.0f, 1.0f),
        glm::vec3(4.0f, 0.0f, 4.0f),
        glm::vec3(1.0f, 1.0f, 4.0f),
        glm::vec3(0.0f, 2.0f, 0.0f)};
    // 透明物体的
    vector<glm::vec3> blendPositions{
        glm::vec3(-1.5f, 0.5f, -0.48f),
        glm::vec3(1.5f, 0.5f, 0.51f),
        glm::vec3(0.0f, 0.5f, 0.7f),
        glm::vec3(-0.3f, 0.5f, -2.3f),
        glm::vec3(0.5f, 0.5f, -0.6f)};

    bool bloom = true;
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("press alt");
        ImGui::Checkbox("bloom", &bloom);
        ImGui::End();

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        // 渲染指令
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO); // 使用创建的帧缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // mvp矩阵
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

        sceneShader.use();
        sceneShader.setMat4("view", view);
        sceneShader.setMat4("projection", projection);

        // 正常绘制地板
        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
        sceneShader.setFloat("uvScale", 4.0f);
        sceneShader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodMap);
        drawMesh(groundGeometry);

        // 绘制2个正方体砖块
        glBindTexture(GL_TEXTURE_2D, brickMap);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0, 1.0, -1.0));
        model = glm::scale(model, glm::vec3(2.0, 2.0, 2.0));
        sceneShader.setMat4("model", model);
        drawMesh(boxGeometry);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0, 0.5, 2.0));
        sceneShader.setMat4("model", model);
        drawMesh(boxGeometry);

        // 绘制透明物体
        glBindTexture(GL_TEXTURE_2D, blendMap);
        // 对透明物体进行动态排序
        std::map<float, glm::vec3> sorted;
        sceneShader.setFloat("uvScale", 1.0f);
        for (unsigned int i = 0; i < blendPositions.size(); i++)
        {
            // 按照距离相机的长度排序透明物体
            float distance = glm::length(camera.Position - blendPositions[i]);
            sorted[distance] = blendPositions[i];
        }
        // rbegin  从远到近绘制物体
        for (std::map<float, glm::vec3>::reverse_iterator iterator = sorted.rbegin(); iterator != sorted.rend(); iterator++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, iterator->second);
            sceneShader.setMat4("model", model);
            drawMesh(blendGeometry);
        }

        // 绘制灯光物体
        float radius = 5.0f;
        float camX = sin(glfwGetTime() * 0.5) * radius;
        float camZ = cos(glfwGetTime() * 0.5) * radius;
        glm::vec3 lightPos = glm::vec3(lightPosition.x * glm::sin(glfwGetTime()) * 2.0, lightPosition.y, lightPosition.z);
        sceneShader.setVec3("directionLight.direction", lightPos); // 光源位置
        sceneShader.setVec3("viewPos", camera.Position);
        pointLightPositions[0].z = camZ;
        pointLightPositions[0].x = camX;
        for (unsigned int i = 0; i < 4; i++)
        {
            // 设置点光源属性
            sceneShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
            sceneShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.01f, 0.01f, 0.01f);
            sceneShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLightColors[i]);
            sceneShader.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);

            // // 设置衰减
            sceneShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
            sceneShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
            sceneShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
        }

        lightObjectShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        lightObjectShader.setMat4("model", model);
        lightObjectShader.setMat4("view", view);
        lightObjectShader.setMat4("projection", projection);
        lightObjectShader.setVec3("lightColor", glm::vec3(2.0f, 2.0f, 2.0f));
        drawMesh(pointLightGeometry);

        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            lightObjectShader.setMat4("model", model);
            lightObjectShader.setVec3("lightColor", pointLightColors[i]);
            drawMesh(pointLightGeometry);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2.高斯模糊明亮的片段
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 10;
        blurShader.use();
        for (unsigned int i = 0; i < amount; i++) // 执行10次模糊操作
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            blurShader.setInt("horizontal", horizontal);
            // 第一次渲染colorBuffers[1](提取的高亮)  之后每次都是对高亮进行模糊处理
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);
            drawMesh(quadGeometry);

            horizontal = !horizontal; // 水平/垂直交替进行模糊操作
            if (first_iteration)      // 是否是第一次
            {
                first_iteration = false;
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3.绘制hdr输出的texture
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        finalShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]); // 0号纹理是物体普通渲染的颜色
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]); // 1号纹理是提取并模糊后的高光
        finalShader.setBool("bloom", bloom);
        finalShader.setFloat("exposure", 1.0);
        drawMesh(quadGeometry);

        // 开始渲染imgui  写在图形后面，才不会被图形遮挡
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    boxGeometry.dispose();
    blendGeometry.dispose();
    pointLightGeometry.dispose();
    groundGeometry.dispose();
    glfwTerminate();

    return 0;
}

// 绘制物体
void drawMesh(BufferGeometry geometry)
{
    glBindVertexArray(geometry.VAO);
    glDrawElements(GL_TRIANGLES, geometry.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// 绘制灯光物体
void drawLightObject(Shader shader, BufferGeometry geometry, glm::vec3 position)
{
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    // // 绘制灯光物体
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    shader.setMat4("model", model);
    shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    drawMesh(geometry);
}

// 窗口变动监听
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

bool isTabPressed = false; // 用来标记Tab键是否按下过
int last_fov = 0;
bool isCursorLocked = true;

/// @brief 按键回调函数
void processInput(GLFWwindow *window)
{
    // ESC
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

// 加载纹理贴图
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    // 图像y轴翻转
    stbi_set_flip_vertically_on_load(true);
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

// 加载立方体贴图
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // 此处需要将y轴旋转关闭，若之前调用过loadTexture
    stbi_set_flip_vertically_on_load(false);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void drawSkyBox(Shader shader, BoxGeometry geometry, unsigned int cubeMap)
{

    glDepthFunc(GL_LEQUAL);
    glDisable(GL_DEPTH_TEST);

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    shader.use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // 移除平移分量

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
    glBindVertexArray(geometry.VAO);
    glDrawElements(GL_TRIANGLES, geometry.indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    view = camera.GetViewMatrix();
}
