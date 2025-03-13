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

string Shader::dirName;
int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 900;
float deltaTime = 0.0f;
float lastTime = 0.0f;
float lastX = SCREEN_WIDTH / 2.0f; // �����һ֡��λ��
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

    // ���ڶ���
    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    {
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        ImGui::StyleColorsDark(); // ��ɫ
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        const char *glsl_version = "#version 330";
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    // �����ӿ�
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    // ��Ȳ���
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // �������¼�
    // 1.ע�ᴰ�ڱ仯����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Shader sceneShader("./shader/scene_vert.glsl", "./shader/scene_frag.glsl"); //��������ͼ
    Shader sceneShader("./shader/scene_vert.glsl", "./shader/scene_texture_frag.glsl"); // ʹ����ͼ
    Shader lightObjShader("./shader/light_object_vert.glsl", "./shader/light_object_frag.glsl");

    SphereGeometry pointLightGeometry(0.17, 64.0, 64.0);       // ���Դλ����ʾ
    SphereGeometry objectGeometry(1.0, 64.0, 64.0);            // Բ��
    glm::vec4 clear_color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f); // ������ɫ

    // ���Դ��λ��
    vector<glm::vec3> lightPositions{
        glm::vec3(-10.0f, 10.0f, 10.0f),
        glm::vec3(10.0f, 10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3(10.0f, -10.0f, 10.0f),
    };

    // ���Դ��ɫ
    vector<glm::vec3> lightColors{
        glm::vec3(300.0f, 400.0f, 100.0f),
        glm::vec3(100.0f, 200.0f, 500.0f),
        glm::vec3(400.0f, 300.0f, 200.0f),
        glm::vec3(500.0f, 200.0f, 200.0f),
    };

    // ������������ = 7*7
    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5; // ����֮��Ŀ�϶

    sceneShader.use();
    sceneShader.setVec3("albedo", 0.0f, 0.5f, 0.0f); // ������
    sceneShader.setFloat("ao", 1.0f);                // �������ڱ�

    // ʹ����ͼ
    // ��ͼ1
    // unsigned int albedoMap = loadTexture("./static/texture/solar/TexturesCom_PaintedConcreteFloor_1K_albedo.png");
    // unsigned int normalMap = loadTexture("./static/texture/solar/TexturesCom_PaintedConcreteFloor_1K_normal.png");
    // unsigned int metallicMap = loadTexture("./static/texture/solar/TexturesCom_PaintedConcreteFloor_1K_metallic.png");
    // unsigned int roughnessMap = loadTexture("./static/texture/solar/TexturesCom_PaintedConcreteFloor_1K_roughness.png");
    // unsigned int aoMap = loadTexture("./static/texture/solar/TexturesCom_PaintedConcreteFloor_1K_ao.png");

    // ��ͼ2
    unsigned int albedoMap = loadTexture("./static/texture/tiles/TexturesCom_Marble_TilesSquare8_512_albedo.png");
    unsigned int normalMap = loadTexture("./static/texture/tiles/TexturesCom_Marble_TilesSquare8_512_normal.png");
    unsigned int roughnessMap = loadTexture("./static/texture/tiles/TexturesCom_Marble_TilesSquare8_512_roughness.png");
    unsigned int metallicMap = 0;
    unsigned int aoMap = 0;

    // ������ͼ
    sceneShader.setInt("albedoMap", 0);
    sceneShader.setInt("normalMap", 1);
    sceneShader.setInt("metallicMap", 2);
    sceneShader.setInt("roughnessMap", 3);
    sceneShader.setInt("aoMap", 4);

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

        // ��Ⱦָ��
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // mvp����
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

        sceneShader.use();
        sceneShader.setMat4("projection", projection);
        sceneShader.setMat4("view", view);
        sceneShader.setVec3("camPos", camera.Position);

        // �����Դλ�� �� ��ɫ
        float radius = 5.0f;
        float camX = sin(glfwGetTime() * 0.5) * radius;
        float camZ = cos(glfwGetTime() * 0.5) * radius;
        lightPositions[1].x = camX; // �õ�һ����Դ�ƶ�
        lightPositions[1].y = camZ;
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            sceneShader.use();
            sceneShader.setVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
            sceneShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
        }

        // ������ͼ
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedoMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, metallicMap);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, roughnessMap);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, aoMap);

        // ��������
        // ���Բ�ʹ����ͼ��ֱ�Ӵ���metallic roughness  �����Ѿ�������ao �� albedo
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

        // ���ƹ�Դ
        lightObjShader.use();
        lightObjShader.setMat4("view", view);
        lightObjShader.setMat4("projection", projection);
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPositions[i]);
            lightObjShader.setMat4("model", model);
            lightObjShader.setVec3("lightColor", lightColors[i]);
            drawMesh(pointLightGeometry);
        }

        // ��ʼ��Ⱦimgui  д��ͼ�κ��棬�Ų��ᱻͼ���ڵ�
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    pointLightGeometry.dispose();
    objectGeometry.dispose();
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
    // ESC
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
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    if (isCursorLocked)
        camera.ProcessMouseMovement(xoffset, yoffset);
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
        cout << "Texture failed to load at path: " << path << endl;
        stbi_image_free(data);
    }

    return textureID;
}

// ������������ͼ
unsigned int loadCubemap(vector<string> faces)
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
