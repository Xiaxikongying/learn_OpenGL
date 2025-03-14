#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <random>
#include <geometry/BoxGeometry.h>
#include <geometry/PlaneGeometry.h>
#include <geometry/SphereGeometry.h>

#include <tool/shader.h>
#include <tool/camera.h>
#include <tool/mesh.h>
#include <tool/model.h>
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
unsigned int loadHdrTexture(char const *path);

string Shader::dirName;
int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 900;
float deltaTime = 0.0f;
float lastTime = 0.0f;
float lastX = SCREEN_WIDTH / 2.0f; // 鼠标上一帧的位置
float lastY = SCREEN_HEIGHT / 2.0f;

Camera camera(glm::vec3(0.0, 1.0, 20.0));

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
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    // 设置视口
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    // 启用混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // 确保立方体正确采样，去除边缘接缝
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // 鼠标键盘事件
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Shader sceneShader("./shader/scene_vert.glsl", "./shader/scene_frag.glsl");                  // 绘制物体
    Shader lightObjShader("./shader/light_object_vert.glsl", "./shader/light_object_frag.glsl"); // 绘制光源
    Shader cubemapShader("./shader/cubemap_vert.glsl", "./shader/cubemap_frag.glsl");            // 将hdr绘制到立方体贴图上
    Shader envmapShader("./shader/envmap_vert.glsl", "./shader/envmap_frag.glsl");               // 绘制天空盒
    Shader irradianceShader("./shader/irradiance_vert.glsl", "./shader/irradiance_frag.glsl");   // 将天空盒贴图转换为光照强度贴图

    SphereGeometry pointLightGeometry(0.17, 64.0, 64.0); // 点光源位置显示
    SphereGeometry objectGeometry(1.0, 64.0, 64.0);      // 圆球
    BoxGeometry boxGeometry(5.0, 5.0, 5.0);

    glm::vec4 clear_color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f); // 背景颜色

    // 点光源的位置
    vector<glm::vec3> lightPositions{
        glm::vec3(-10.0f, 10.0f, 10.0f),
        glm::vec3(10.0f, 10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3(10.0f, -10.0f, 10.0f),
    };

    // 点光源颜色
    vector<glm::vec3> lightColors{
        glm::vec3(300.0f, 400.0f, 100.0f),
        glm::vec3(100.0f, 200.0f, 500.0f),
        glm::vec3(400.0f, 300.0f, 200.0f),
        glm::vec3(500.0f, 200.0f, 200.0f),
    };

    // 绘制物体数量 = 7*7
    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5; // 物体之间的空隙

    sceneShader.use();
    sceneShader.setInt("irradianceMap", 0);
    sceneShader.setVec3("albedo", 0.0f, 0.5f, 0.0f); // 反射率
    sceneShader.setFloat("ao", 1.0f);                // 环境光遮蔽

    envmapShader.use();
    envmapShader.setInt("envMap", 0);

    // 生成一个帧缓冲captureFBO
    unsigned int captureFBO;
    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO); // 使用帧缓冲captureFBO

    // 生成一个深度缓存captureRBO
    unsigned int captureRBO;
    glGenRenderbuffers(1, &captureRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512); // 绑定深度
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // 为六个面分配内存  颜色缓存
    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap); // 绑定为立方体贴图
    for (unsigned int i = 0; i < 6; ++i)
    {
        // 使用16位浮点数存储
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载立方体贴图（HDR贴图：可表示整个360°全景环境）
    unsigned int hdrMap = loadHdrTexture("./static/texture/Alexs_Apt_2k.hdr");
    // unsigned int hdrMap = loadHdrTexture("./static/texture/Ditch-River_2k.hdr");
    cubemapShader.use();
    cubemapShader.setInt("equireMap", 0);
    // 将等距柱状纹理捕捉到立方体贴图的每个面
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::vec3 lookEye = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    };

    // 将hdr环境图转换为等效的立方体贴图
    cubemapShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrMap); // 使用hdr贴图
    glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO); // 使用帧缓冲captureFBO
    for (unsigned int i = 0; i < 6; i++)
    {
        cubemapShader.setMat4("view", captureViews[i]); // 相机视角为朝向正方体的六个面
        // 当前渲染的内容应该存到 envCubemap的第i个面上
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawMesh(boxGeometry); // 将立方体的信息绘制到envCubemap的第i个面上
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // 切换到默认帧缓冲

    // 创建辐照度立方体贴图
    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // 创建光照强度的立方体贴图
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);   // 使用captureFBO帧缓冲
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO); // 使用captureRBO深度缓存
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
    irradianceShader.use();
    irradianceShader.setInt("envMap", 0);
    irradianceShader.setMat4("projection", captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap); // 使用hdr贴图转化为的立方体贴图
    glViewport(0, 0, 32, 32);                       // 设置视口 分辨率32*32（即光照强度贴图的渲染大小为32*32）
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawMesh(boxGeometry); // 使用irradianceShader将envCubemap绘制到irradianceMap中
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // 使用默认的帧缓冲

    // 恢复原来的窗口渲染尺寸
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight); // 获取窗口大小
    glViewport(0, 0, scrWidth, scrHeight);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        // imgui
        {
            int fps_value = (int)round(ImGui::GetIO().Framerate);
            int ms_value = (int)round(1000.0f / ImGui::GetIO().Framerate);
            std::string FPS = std::to_string(fps_value);
            std::string ms = std::to_string(ms_value);
            std::string newTitle = "LearnOpenGL - " + ms + " ms/frame " + FPS;
            glfwSetWindowTitle(window, newTitle.c_str());
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        // 渲染指令
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // mvp矩阵
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

        sceneShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // 使用光照强度贴图

        float radius = 5.0f;
        float camX = sin(glfwGetTime() * 0.5) * radius;
        float camZ = cos(glfwGetTime() * 0.5) * radius;
        lightPositions[1].x = camX; // 移动第一个光源
        lightPositions[1].y = camZ;
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            sceneShader.setVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
            sceneShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
        }
        sceneShader.setMat4("projection", projection);
        sceneShader.setMat4("view", view);
        sceneShader.setVec3("camPos", camera.Position);
        // 绘制7*7个物体
        for (int row = 0; row < nrRows; ++row)
        {
            sceneShader.setFloat("metallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; ++col)
            {
                sceneShader.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3((col - (nrColumns / 2)) * spacing, (row - (nrRows / 2)) * spacing, 0.0f));
                sceneShader.setMat4("model", model);
                drawMesh(objectGeometry);
            }
        }

        // 绘制hdr立方体  (测试使用)
        // cubemapShader.use();
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, hdrMap);
        // cubemapShader.setMat4("view", view);
        // cubemapShader.setMat4("projection", projection);
        // drawMesh(boxGeometry); // 绘制hdr立方体  (测试使用)

        // 使用处理之后的环境贴图

        envmapShader.use();
        envmapShader.setMat4("view", view);
        envmapShader.setMat4("projection", projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap); // 显示hdr转化的立方体贴图（即天空盒）
        // glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // 显示生成的光照贴图（天空盒生成的）
        drawMesh(boxGeometry); // 绘制天空盒

        // 绘制灯光物体

        lightObjShader.use();
        lightObjShader.setMat4("view", view);
        lightObjShader.setMat4("projection", projection);
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            lightObjShader.setMat4("model", model);
            lightObjShader.setVec3("lightColor", lightColors[i]);
            drawMesh(pointLightGeometry);
        }

        // 开始渲染imgui  写在图形后面，才不会被图形遮挡
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    boxGeometry.dispose();
    pointLightGeometry.dispose();
    objectGeometry.dispose();
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
        cout << "Texture failed to load at path: " << path << endl;
        stbi_image_free(data);
    }

    return textureID;
}

// 加载立方体贴图
unsigned int loadCubemap(vector<string> faces)
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
            cout << "Cubemap texture failed to load at path: " << faces[i] << endl;
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

// 加载hdr贴图
unsigned int loadHdrTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    // 图像y轴翻转
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
