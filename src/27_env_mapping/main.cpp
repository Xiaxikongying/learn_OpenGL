#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <geometry/BoxGeometry.h>
#include <geometry/PlaneGeometry.h>
#include <geometry/SphereGeometry.h>
#include <tool/shader.h>
#include <tool/camera.h>
#include <tool/gui.h>
#include <tool/mesh.h>
#include <tool/model.h>
#include <map>
using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const *path);
void drawSkyBox(Shader shader, BoxGeometry geometry, unsigned int cubeMap);
unsigned int loadCubemap(vector<std::string> faces);

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

    // �����ӿ�
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    // ���
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ��Ȳ���
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // �������¼�
    // 1.ע�ᴰ�ڱ仯����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 2.����¼�
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Shader reflectShader("./shader/reflect_object_vert.glsl", "./shader/reflect_object_frag.glsl"); // ����
    Shader refractShader("./shader/refract_object_vert.glsl", "./shader/refract_object_frag.glsl"); // ����
    Shader skyboxShader("./shader/cube_map_vert.glsl", "./shader/cube_map_frag.glsl");

    PlaneGeometry groundGeometry(10.0, 10.0);  // ����
    PlaneGeometry blendGeometry(1.0, 1.0);     // ͸������
    BoxGeometry boxGeometry(1.0, 1.0, 1.0);    // ����
    BoxGeometry skyboxGeometry(1.0, 1.0, 1.0); // ��պ�

    unsigned int woodMap = loadTexture("./static/texture/wood.png");                         // ������ͼ
    unsigned int brickMap = loadTexture("./static/texture/brick_diffuse.jpg");               // ש����ͼ
    unsigned int blendMap = loadTexture("./static/texture/blending_transparent_window.png"); // ͸��������ͼ

    ImVec4 clear_color = ImVec4(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0); // 25, 25, 25

    // ��պ���ͼ
    vector<string> faces{
        "./static/texture/Park3Med/px.jpg",
        "./static/texture/Park3Med/nx.jpg",
        "./static/texture/Park3Med/py.jpg",
        "./static/texture/Park3Med/ny.jpg",
        "./static/texture/Park3Med/pz.jpg",
        "./static/texture/Park3Med/nz.jpg"};

    unsigned int cubemapTexture = loadCubemap(faces);
    Model ourModel("./static/model/walt/WaltHead.obj");
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        // ��Ⱦָ��
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // ������պ�
        drawSkyBox(skyboxShader, skyboxGeometry, cubemapTexture);

        // ��ʼ��mvp����
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
        model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

        refractShader.use();
        refractShader.setMat4("model", model);
        refractShader.setMat4("view", view);
        refractShader.setMat4("projection", projection);
        refractShader.setVec3("cameraPos", camera.Position);
        refractShader.setVec3("objectColor", glm::vec3(0.0, 0.0, 0.0));

        ourModel.Draw(refractShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0, 0.0, 0.0));
        model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
        refractShader.setMat4("model", model);
        refractShader.setVec3("objectColor", glm::vec3(0.1, 0.0, 0.1));
        ourModel.Draw(refractShader);

        reflectShader.use();
        reflectShader.setMat4("view", view);
        reflectShader.setMat4("projection", projection);
        reflectShader.setVec3("cameraPos", camera.Position);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0, 0.0, 0.0));
        model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
        reflectShader.setMat4("model", model);
        reflectShader.setVec3("objectColor", glm::vec3(0.1, 0.1, 0.0));
        ourModel.Draw(reflectShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    boxGeometry.dispose();
    blendGeometry.dispose();
    groundGeometry.dispose();
    glfwTerminate();

    return 0;
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

/// @brief ������������ͼ
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

/// @brief ������պ�
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
