#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <geometry/BoxGeometry.h>
#include <geometry/SphereGeometry.h>

#include <tool/shader.h>
#include <tool/gui.h>
#include <tool/camera.h>
#include <tool/mesh.h>
#include <tool/model.h>

#include <math.h>
#include <iostream>
#include <vector>

using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_calback(GLFWwindow *window, int button, int action, int mods);
void scroll_callback(GLFWwindow *window, double x, double y);

// ������ͼ
unsigned int loadTexture(char const *path);

string Shader::dirName;

int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 900;

float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��

float lastX = SCREEN_WIDTH / 2.0f; // �����һ֡��λ��
float lastY = SCREEN_HEIGHT / 2.0f;

// ��ʼ�����
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f)); // ���λ��

int main(int agrc, char *argv[])
{
    Shader::dirName = argv[1]; // ex: argv[1] = "src/05_shader_class"
    Shader::dirName += "/";

    glfwInit();                                                    // ��ʼ��GLFW ,����Ҫ���ڴ���/�����ڣ������û������
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 // ��Ҫ�汾
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 // ��Ҫ�汾  ��3.3�汾��opengl
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // ����ģʽ

    // ��glfw����   ���ڴ�С��������
    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);                                // ������ɫ���
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // �趨���ģʽ��ʵ�� ���� alpha ͸���ȵ���ɫ���
    glEnable(GL_DEPTH_TEST);                           // ������Ȳ���

    // ע��ص�����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //--------------imgui����--------------
    // ����GUI������
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // ����imgui������ɫ
    ImGui::StyleColorsDark(); // ��ɫ   //ImGui::StyleColorsClassic();//����Ƥ��

    // ����ƽ̨
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char *glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);
    //--------------imgui�������--------------

    // ������ɫ��
    Shader ourShader("./shader/vertex.glsl", "./shader/fragment.glsl");
    Shader lightShader("./shader/light_vert.glsl", "./shader/light_frag.glsl");

    SphereGeometry dirLight(0.5f);
    vector<SphereGeometry> pointLight(4, SphereGeometry(0.2f)); // ���Դ

    float value = 0; // ʱ��ֵ
    float aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
    glm::vec3 clearColor = glm::vec3(0.2f, 0.3f, 0.5f);  // ������ɫ
    glm::vec3 cubeColor = glm::vec3(1.0f, 1.0f, 1.0f);   // ��������ɫ
    glm::vec3 sphereColor = glm::vec3(0.8f, 0.2f, 0.5f); // ����ɫ

    // ���������� ���淴����ͼ
    unsigned int diffuseMap = loadTexture("./static/texture/container2.png");
    unsigned int specularMap = loadTexture("./static/texture/container2_specular.png");

    // ����ʹ��������ͼ
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    ourShader.use();
    // ���ݲ�������
    ourShader.setInt("material.diffuse", 0);
    ourShader.setInt("material.specular", 1);
    ourShader.setFloat("material.shininess", 32.0f);

    // ���ù�������
    // ����ƽ�й��������
    glm::vec3 directionLightPos = glm::vec3(2.0f, 1.0f, 3.0f); // ��Դλ��
    glm::vec3 directionLightColor = glm::vec3(0.2f, 0.2f, 0.2f);
    ourShader.setVec3("directionLight.ambient", 0.01f, 0.01f, 0.01f);
    ourShader.setVec3("directionLight.diffuse", directionLightColor); // �����յ�����һЩ�Դ��䳡��
    ourShader.setVec3("directionLight.specular", 1.0f, 1.0f, 1.0f);
    ourShader.setVec3("directionLight.direction", directionLightPos);

    // ���þ۹��������
    ourShader.setVec3("spotLight.position", camera.Position);
    ourShader.setVec3("spotLight.direction", camera.Front);
    ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    ourShader.setFloat("spotLight.constant", 1.0f);
    ourShader.setFloat("spotLight.linear", 0.09);
    ourShader.setFloat("spotLight.quadratic", 0.032);
    ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    // ����˥��
    ourShader.setFloat("light.constant", 1.0f);
    ourShader.setFloat("light.linear", 0.09f);
    ourShader.setFloat("light.quadratic", 0.032f);

    // ���Դ��λ�� / ��ɫ
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 1.5f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    glm::vec3 pointLightColors[] = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)};

    // ���õ��Դ����
    for (unsigned int i = 0; i < 4; i++)
    {
        ourShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
        ourShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.01f, 0.01f, 0.01f);
        ourShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLightColors[i]);
        ourShader.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);

        // // ����˥��
        ourShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
        ourShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
        ourShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
    }

    // Model ourModel("./static/model/cerberus/Cerberus.obj");
    Model ourModel("./static/model/nanosuit/nanosuit.obj");

    // ѭ����Ⱦ
    while (!glfwWindowShouldClose(window)) // �����Ƿ�ر�
    {
        processInput(window); // ����Ƿ�������

        {
            // ����imgui����
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // ������Ⱦ����
            ImGui::Begin("Camera");
            // ��ʾ֡��
            ImGui::Text("%.3f ms-fram (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            // ��ʾ������
            ImGui::SliderFloat("fov", &camera.Zoom, 1.0f, 60.0f);
            ImGui::SliderFloat("mouseSensitivity", &camera.MouseSensitivity, 0.01f, 0.5f);
            ImGui::SliderFloat("view.x", &camera.Position.x, -30.0f, 30.0f); // ��Χ
            ImGui::SliderFloat("view.y", &camera.Position.y, -30.0f, 30.0f);
            ImGui::SliderFloat("view.z", &camera.Position.z, -30.0f, 30.0f);
            ImGui::End();
        }

        // ��Ⱦָ�� ....
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        // �����ɫ����  �����Ȼ���
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        value = glfwGetTime();
        deltaTime = value - lastFrame; // ��������һ֮֡���ʱ���
        lastFrame = value;             // ��¼��ǰ֡��ʱ��

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // model����
        glm::mat4 model = glm::mat4(1.0f);
        // view����
        glm::mat4 view = camera.GetViewMatrix();
        // projection����
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), aspect, 0.1f, 100.0f);

        // ������ɫ��
        ourShader.use();
        ourShader.setMat4("model", model);
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setVec3("objectColor", cubeColor);

        // ���¾۹��λ��
        ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        // ƽ�й���ɫ
        ourShader.setVec3("directionLight.diffuse", directionLightColor);

        ourModel.Draw(ourShader);

        // ���Ƶ��Դ
        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        for (int i = 0; i < pointLight.size(); ++i)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            lightShader.setMat4("model", model);
            lightShader.setVec3("lightColor", pointLightColors[i]);
            glBindVertexArray(pointLight[i].VAO);
            glDrawElements(GL_TRIANGLES, pointLight[i].indices.size(), GL_UNSIGNED_INT, 0);
        }

        // ����ƽ�й�Դ
        model = glm::mat4(1.0f);
        model = glm::translate(model, directionLightPos);
        lightShader.setMat4("model", model);
        lightShader.setVec3("lightColor", directionLightColor);
        glBindVertexArray(dirLight.VAO);
        glDrawElements(GL_TRIANGLES, dirLight.indices.size(), GL_UNSIGNED_INT, 0);

        // ��ʼ��Ⱦimgui  д��ͼ�κ��棬�Ų��ᱻͼ���ڵ�
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window); // ������ɫ����
        glfwPollEvents();        // ����Ƿ����¼�
    }

    glfwTerminate(); // �ر�glfw
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // �ı��ӿڴ�С
    glViewport(0, 0, width, height);
}

bool isTabPressed = false; // �������Tab���Ƿ��¹�
int last_fov = 0;

/// @brief �����ص�����
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
