#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <map>

#include <tool/shader.h>
#include <tool/camera.h>
#include <geometry/BoxGeometry.h>
#include <geometry/PlaneGeometry.h>
#include <geometry/SphereGeometry.h>

#include <tool/gui.h>
#include <tool/mesh.h>
#include <tool/model.h>
using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const *path);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// method
void drawMesh(BufferGeometry geometry);
void drawLightObject(Shader shader, BufferGeometry geometry, glm::vec3 position);

string Shader::dirName;

int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 900;

// delta time
float deltaTime = 0.0f;
float lastTime = 0.0f;
float lastX = SCREEN_WIDTH / 2.0f; // �����һ֡��λ��
float lastY = SCREEN_HEIGHT / 2.0f;
Camera camera(glm::vec3(0.0, 1.0, 6.0));

int main(int argc, char *argv[])
{
    Shader::dirName = argv[1];
    Shader::dirName += "/";
    glfwInit();
    // ������Ҫ�ʹ�Ҫ�汾
    const char *glsl_version = "#version 330";
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ���ڶ���
    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    {
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        // ������ʽ
        ImGui::StyleColorsDark();
        // ����ƽ̨����Ⱦ��
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    // �����ӿ�
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // �������޳�
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // �������¼�
    // 1.ע�ᴰ�ڱ仯����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Shader sceneShader("./shader/scene_vert.glsl", "./shader/scene_frag.glsl");
    Shader lightObjectShader("./shader/light_object_vert.glsl", "./shader/light_object_frag.glsl");
    Shader simpleShadowShader("./shader/shadow_map_vert.glsl", "./shader/shadow_map_frag.glsl");
    Shader finalShaderShader("./shader/shadow_final_vert.glsl", "./shader/shadow_final_frag.glsl");
    Shader quadShader("./shader/shadow_quad_vert.glsl", "./shader/shadow_quad_frag.glsl");

    PlaneGeometry groundGeometry(10.0, 10.0);            // ����
    PlaneGeometry quadGeometry(6.0, 6.0);                // �������
    BoxGeometry boxGeometry(1.0, 1.0, 1.0);              // ����
    BoxGeometry floorGeometry(10.0, 0.0001, 10.0);       // ����
    SphereGeometry pointLightGeometry(0.06, 10.0, 10.0); // ���Դλ����ʾ

    unsigned int woodMap = loadTexture("./static/texture/wood.png");           // ����
    unsigned int brickMap = loadTexture("./static/texture/brick_diffuse.jpg"); // שǽ

    // ------------------------------------------------
    // ����֡����
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // ����һ��2D����
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // ���ɵ�������Ϊ֡�������Ȼ���
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    quadShader.use();
    quadShader.setInt("depthMap", 0);

    // ------------------------------------------------
    finalShaderShader.use();
    finalShaderShader.setInt("diffuseTexture", 0);
    finalShaderShader.setInt("shadowMap", 1);
    glm::vec3 lightPosition = glm::vec3(-2.0f, 3.0f, -1.0f); // ����λ��

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

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

        // ��Ⱦָ��
        // ...
        glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightPosition = glm::vec3(lightPosition.x + glm::sin(glfwGetTime()) * 0.03, lightPosition.y, lightPosition.z);
        glm::mat4 model = glm::mat4(1.0f);

        // ��Ⱦ�����ͼ
        glm::mat4 lightProjection;  // ��Դ����������
        glm::mat4 lightView;        // ��Դ���������ĵ�����---->��ʾƽ�й�ķ���
        glm::mat4 lightSpaceMatrix; // ��ԴͶӰ����
        float near_plane = 1.0f, far_plane = 7.5f;
        // ǰ4�����ֱ�ʾͶӰ�ռ�ı߽�
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //
        lightSpaceMatrix = lightProjection * lightView;
        simpleShadowShader.use();
        simpleShadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        // 1. ��ѡ��Ⱦ�����ͼ
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);  // �ӿڴ�С��ȷ����Ⱦ����Ӱ��ͼ�� 1024x1024
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); // ����Ӱ֡���壬�������Ļ��ƻ�д��depthMap����
        glClear(GL_DEPTH_BUFFER_BIT);                   // �����Ȼ��壬��ֹ�������ݸ�����Ӱ���㡣
        glActiveTexture(GL_TEXTURE0);
        // ���Ƴ���  ʹ��simpleShadowShader��������ȫ��ͶӰ����Դ������������
        // ���Ƶذ���Ӱ��ͼ
        glBindTexture(GL_TEXTURE_2D, woodMap);
        simpleShadowShader.setMat4("model", model);
        drawMesh(floorGeometry); // ��ʱ�Ὣ������ԴͶӰ�����ĵ�д����Ȼ��壬��������Ļ
        // ����������Ӱ��ͼ
        glBindTexture(GL_TEXTURE_2D, brickMap);
        model = glm::translate(model, glm::vec3(0.0, 0.5, 0.0));
        simpleShadowShader.setMat4("model", model);
        drawMesh(boxGeometry);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. ������һ����Ⱦ�����������ʹ�������ͼ
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

        finalShaderShader.use();
        finalShaderShader.setMat4("view", view);
        finalShaderShader.setMat4("projection", projection);
        finalShaderShader.setVec3("viewPos", camera.Position);
        finalShaderShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        finalShaderShader.setVec3("lightPos", lightPosition); // ��Դλ��
        finalShaderShader.setFloat("uvScale", 4.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        model = glm::mat4(1.0f);
        finalShaderShader.setMat4("model", model);
        drawMesh(floorGeometry);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, brickMap);
        model = glm::translate(model, glm::vec3(0.0, 0.5, 0.0));
        finalShaderShader.setMat4("model", model);
        finalShaderShader.setFloat("uvScale", 1.0f);
        drawMesh(boxGeometry);

        // ��ʾ�����ͼ
        // quadShader.use();
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, depthMap); // ʹ�������ͼ
        // quadShader.setMat4("view", view);

        // model = glm::mat4(1.0f);
        // quadShader.setFloat("near_plane", near_plane);
        // quadShader.setFloat("far_plane", far_plane);
        // quadShader.setMat4("model", model);
        // quadShader.setMat4("projection", projection);
        // drawMesh(quadGeometry);

        drawLightObject(lightObjectShader, pointLightGeometry, lightPosition);

        // ��Ⱦ gui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    groundGeometry.dispose();
    pointLightGeometry.dispose();
    glfwTerminate();

    return 0;
}

// ��������
void drawMesh(BufferGeometry geometry)
{
    glBindVertexArray(geometry.VAO);
    glDrawElements(GL_TRIANGLES, geometry.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// ���Ƶƹ�����
void drawLightObject(Shader shader, BufferGeometry geometry, glm::vec3 position)
{
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    // // ���Ƶƹ�����
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    shader.setMat4("model", model);
    shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    drawMesh(geometry);
}

// ���ڱ䶯����
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

bool isTabPressed = false; // �������Tab���Ƿ��¹�
int last_fov = 0;
bool isCursorLocked = true;

/// @brief �����ص�����
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
    {
        // ALT ���£���ʾ���
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        isCursorLocked = false;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE && !isCursorLocked)
    {
        // ALT �ɿ����������
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

    // �Ŵ��ӽ�
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !isTabPressed)
    {
        last_fov = camera.Zoom;
        camera.Zoom = 10.0f;
        isTabPressed = true; // ��� Tab ���Ѱ���
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE && isTabPressed)
    {
        camera.Zoom = last_fov;
        isTabPressed = false; // ��� Tab ����̧��
    }
}

/// @brief ����ƶ��ص�����
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (isCursorLocked)
    {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

/// @brief �����ֻص�����
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.Zoom -= (float)yoffset;
    if (camera.Zoom < 1.0f)
        camera.Zoom = 1.0f;
    if (camera.Zoom > 60.0f)
        camera.Zoom = 60.0f;
    return;
}

// ����������ͼ
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    // ͼ��y�ᷭת
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

// ������������ͼ
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // �˴���Ҫ��y����ת�رգ���֮ǰ���ù�loadTexture
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
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // �Ƴ�ƽ�Ʒ���

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
