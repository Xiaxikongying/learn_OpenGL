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

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);                                // ������ɫ���
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // �趨���ģʽ��ʵ�� ���� alpha ͸���ȵ���ɫ���
    glEnable(GL_DEPTH_TEST);                           // ������Ȳ���

    // ע��ص�����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // ������ɫ��
    Shader ourShader("./shader/vertex.glsl", "./shader/fragment.glsl");
    Shader lightShader("./shader/light_vert.glsl", "./shader/light_frag.glsl");

    BoxGeometry boxGeometry(1.0f, 1.0f, 1.0f);
    SphereGeometry sphereGeometry(0.75f);
    SphereGeometry light(0.2f); // ��Դ

    float value = 0; // ʱ��ֵ
    float aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
    glm::vec3 clearColor = glm::vec3(0.2f, 0.3f, 0.5f);  // ������ɫ
    glm::vec3 cubeColor = glm::vec3(0.6f, 0.8f, 0.2f);   // ��������ɫ
    glm::vec3 sphereColor = glm::vec3(0.8f, 0.2f, 0.5f); // ����ɫ
    glm::vec3 lightPos = glm::vec3(3.0f, 3.0f, 1.0f);    // ��Դλ��
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);  // ��Դ��ɫ

    // ���ݲ�������
    ourShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    ourShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    ourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    ourShader.setFloat("material.shininess", 32.0f);

    // ���ù�������
    ourShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    ourShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    ourShader.setVec3("light.position", lightPos);

    // ѭ����Ⱦ
    while (!glfwWindowShouldClose(window)) // �����Ƿ�ر�
    {
        processInput(window); // ����Ƿ�������
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

        ImGui::Begin("Lighting");
        ImGui::ColorEdit3("cubeColor", (float *)&cubeColor);
        ImGui::ColorEdit3("sphereColor", (float *)&sphereColor);
        ImGui::ColorEdit3("lightColor", (float *)&lightColor);
        ImGui::End();

        // ��Ⱦָ�� ....
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        // �����ɫ����  �����Ȼ���
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        value = glfwGetTime();
        deltaTime = value - lastFrame; // ��������һ֮֡���ʱ���
        lastFrame = value;             // ��¼��ǰ֡��ʱ��

        // model����
        glm::mat4 model = glm::mat4(1.0f);
        // view����
        glm::mat4 view = camera.GetViewMatrix();
        // projection����
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), aspect, 0.1f, 100.0f);

        lightPos.x = sin(value) * 2;
        lightPos.y = cos(value) * 2;

        // ������ɫ��
        ourShader.use();
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setVec3("light.position", lightPos);

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

        ourShader.setVec3("material.ambient", ambientColor);
        ourShader.setVec3("material.diffuse", diffuseColor);

        // ����������
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, value, glm::vec3(value, value, value));
        ourShader.setMat4("model", model);
        ourShader.setVec3("objectColor", cubeColor);
        glBindVertexArray(boxGeometry.VAO);
        glDrawElements(GL_TRIANGLES, boxGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        // ��������
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        ourShader.setVec3("objectColor", sphereColor);
        glBindVertexArray(sphereGeometry.VAO);
        glDrawElements(GL_TRIANGLES, sphereGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        // ��Դ��ɫ��
        lightShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        lightShader.setMat4("model", model);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightShader.setVec3("lightColor", lightColor);

        glBindVertexArray(light.VAO);
        glDrawElements(GL_TRIANGLES, light.indices.size(), GL_UNSIGNED_INT, 0);

        // ��ʼ��Ⱦimgui  д��ͼ�κ��棬�Ų��ᱻͼ���ڵ�
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window); // ������ɫ����
        glfwPollEvents();        // ����Ƿ����¼�
    }
    boxGeometry.dispose();
    sphereGeometry.dispose();
    light.dispose();
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