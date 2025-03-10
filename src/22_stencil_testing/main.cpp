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
using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const *path);

std::string Shader::dirName;

int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 900;

float deltaTime = 0.0f;
float lastTime = 0.0f;

float lastX = SCREEN_WIDTH / 2.0f; // �����һ֡��λ��
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

    // ���ڶ���
    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // -----------------------
    // ����imgui������
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // ������ʽ
    ImGui::StyleColorsDark();
    // ����ƽ̨����Ⱦ��
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // -----------------------

    // �����ӿ�
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ��Ȳ���
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // ģ�����
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // �������¼�
    // 1.ע�ᴰ�ڱ仯����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 2.����¼�
    glfwSetCursorPosCallback(window, mouse_callback);

    Shader sceneShader("./shader/scene_vert.glsl", "./shader/scene_frag.glsl");
    Shader lightObjectShader("./shader/light_vert.glsl", "./shader/light_frag.glsl");

    PlaneGeometry planeGeometry(10.0, 10.0, 10.0, 10.0);
    BoxGeometry boxGeometry(1.0, 1.0, 1.0);
    SphereGeometry sphereGeometry(0.04, 10.0, 10.0);
    SphereGeometry sphereGeometry2(0.5, 50.0, 50.0);

    unsigned int woodMap = loadTexture("./static/texture/wood.png");           // ����
    unsigned int brickMap = loadTexture("./static/texture/brick_diffuse.jpg"); // ש��

    sceneShader.use();
    sceneShader.setInt("brickMap", 0);

    float factor = 0.0;

    glm::vec3 view_translate = glm::vec3(0.0, 0.0, -5.0);
    ImVec4 clear_color = ImVec4(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0); // 25, 25, 25

    glm::vec3 lightPosition = glm::vec3(1.0, 2.5, 2.0); // ����λ��

    // ���ݲ�������
    sceneShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    sceneShader.setFloat("material.shininess", 32.0f);

    // ����ƽ�й��������
    sceneShader.setVec3("directionLight.ambient", 0.01f, 0.01f, 0.01f);
    sceneShader.setVec3("directionLight.diffuse", 0.2f, 0.2f, 0.2f); // �����յ�����һЩ�Դ��䳡��
    sceneShader.setVec3("directionLight.specular", 1.0f, 1.0f, 1.0f);

    // ����˥��
    sceneShader.setFloat("light.constant", 1.0f);
    sceneShader.setFloat("light.linear", 0.09f);
    sceneShader.setFloat("light.quadratic", 0.032f);

    // ���Դ��λ��
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 1.0f, 1.5f),
        glm::vec3(2.3f, 3.0f, -4.0f),
        glm::vec3(-4.0f, 2.0f, 1.0f),
        glm::vec3(1.4f, 2.0f, 0.3f)};
    // ���Դ��ɫ
    glm::vec3 pointLightColors[] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)};

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        // �ڱ�������ʾ֡����Ϣ
        // *************************************************************************
        int fps_value = (int)round(ImGui::GetIO().Framerate);
        int ms_value = (int)round(1000.0f / ImGui::GetIO().Framerate);

        std::string FPS = std::to_string(fps_value);
        std::string ms = std::to_string(ms_value);
        std::string newTitle = "LearnOpenGL - " + ms + " ms/frame " + FPS;
        glfwSetWindowTitle(window, newTitle.c_str());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ��Ⱦָ��
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        sceneShader.use();
        factor = glfwGetTime();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodMap);

        float radius = 5.0f;
        float camX = sin(glfwGetTime() * 0.5) * radius;
        float camZ = cos(glfwGetTime() * 0.5) * radius;

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

        glm::vec3 lightPos = glm::vec3(lightPosition.x * glm::sin(glfwGetTime()) * 2.0, lightPosition.y, lightPosition.z);
        sceneShader.use();
        sceneShader.setMat4("view", view);
        sceneShader.setMat4("projection", projection);

        sceneShader.setVec3("directionLight.direction", lightPos); // ��Դλ��
        sceneShader.setVec3("viewPos", camera.Position);

        pointLightPositions[0].z = camZ;
        pointLightPositions[0].x = camX;

        for (unsigned int i = 0; i < 4; i++)
        {
            // ���õ��Դ����
            sceneShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
            sceneShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.01f, 0.01f, 0.01f);
            sceneShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLightColors[i]);
            sceneShader.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);

            // // ����˥��
            sceneShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
            sceneShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
            sceneShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));

        sceneShader.setMat4("model", model);

        // �������Ƶذ�
        glStencilMask(0x00); // ��ֹд��ģ�建��
        glBindVertexArray(planeGeometry.VAO);
        glDrawElements(GL_TRIANGLES, planeGeometry.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 1.�������ƶ���д��ģ�建����
        glStencilFunc(GL_ALWAYS, 1, 0xff); // ȫ�����ƶ���ͨ��ģ�����  �ɹ�д��ref(�ڶ�������)
        glStencilMask(0xff);               // ����д��ģ�建��

        // ����2��������ש��
        glBindTexture(GL_TEXTURE_2D, brickMap);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0, 1.0, -1.0));
        model = glm::scale(model, glm::vec3(2.0, 2.0, 2.0));
        sceneShader.setMat4("model", model);
        glBindVertexArray(boxGeometry.VAO);
        glDrawElements(GL_TRIANGLES, boxGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0, 0.5, 2.0));
        sceneShader.setMat4("model", model);
        glBindVertexArray(boxGeometry.VAO);
        glDrawElements(GL_TRIANGLES, boxGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        // 2.���Ʊ߿�  ����ģ��д�����Ȳ���
        glStencilFunc(GL_NOTEQUAL, 1, 0xff); // ģ��ֵ������1�ĲŻ�ͨ������
        glStencilMask(0x00);                 // ��ֹд��ģ�建��  ��ģ�建��Ϊ0
        glDisable(GL_DEPTH_TEST);

        sceneShader.setFloat("stenci", 1.0);
        float scale = 1.03; // ��ԭ���������Դ�һ��
        // ��һ���߿�
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0, 1.0, -1.0)); // translate��scale������һ��
        model = glm::scale(model, glm::vec3(2.0 * scale, 2.0 * scale, 2.0 * scale));
        sceneShader.setMat4("model", model);
        glBindVertexArray(boxGeometry.VAO);
        glDrawElements(GL_TRIANGLES, boxGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        // �ڶ����߿�
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0, 0.5, 2.0));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        sceneShader.setMat4("model", model);
        glBindVertexArray(boxGeometry.VAO);
        glDrawElements(GL_TRIANGLES, boxGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glStencilMask(0xff); // ����д��ģ�建��
        glStencilFunc(GL_ALWAYS, 0, 0xff);
        glEnable(GL_DEPTH_TEST);
        sceneShader.setFloat("stenci", 0.0); // ������ɱ߿�   ��ģ��ֵ���Ϊ0

        // ���Ƶƹ�����
        // ************************************************************
        lightObjectShader.use();
        lightObjectShader.setMat4("view", view);
        lightObjectShader.setMat4("projection", projection);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);

        lightObjectShader.setMat4("model", model);
        lightObjectShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        glBindVertexArray(sphereGeometry.VAO);
        glDrawElements(GL_TRIANGLES, sphereGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);

            lightObjectShader.setMat4("model", model);
            lightObjectShader.setVec3("lightColor", pointLightColors[i]);

            glBindVertexArray(sphereGeometry.VAO);
            glDrawElements(GL_TRIANGLES, sphereGeometry.indices.size(), GL_UNSIGNED_INT, 0);
        }
        // ************************************************************

        // ��Ⱦ gui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    boxGeometry.dispose();
    planeGeometry.dispose();
    sphereGeometry.dispose();
    glfwTerminate();

    return 0;
}

// ���ڱ䶯����
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// �����������
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // �����������
    // ����ƶ�
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

// ����ƶ�����
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
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