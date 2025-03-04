#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;

// make run dir=01_hello_window    dir��Ҫ�пո�

/// @brief �ص��������û��ı䴰�ڵĴ�С��ʱ���ӿ�ҲӦ�ñ�����
/// @param window  ����
/// @param width_height ���ڸı��Ĵ�С
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

/// @brief �������
/// @param window ���Ĵ���
void processInput(GLFWwindow *window);

// ������ɫ��  ���������ʹ���ַ�����ʽд
const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
                                 "gl_PointSize = 20.0f;\n"
                                 "}\n\0";
// Ƭ����ɫ��
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\n\0";

int main()
{
    glfwInit();                                                    // ��ʼ��GLFW ,����Ҫ���ڴ���/�����ڣ������û������
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 // ��Ҫ�汾
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 // ��Ҫ�汾  ��3.3�汾��opengl
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // ����ģʽ

    // 1.1����glfw����   ���ڴ�С��������
    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        cout << "failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // ����OpenGL��Ⱦ�ĸ�����

    // 1.2��ʼ��GLAD  glfwGetProcAddress��OpenGL����ָ���ַ�ĺ���
    //  GLAD �����ѯ OpenGL �������򣬲���ȡ��ȷ�� API ������ַ
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // 1.3�����ӿڳߴ�  ǰ�������ƴ������½ǵ�λ�á�3��4��ʾ���ڵĿ�͸�
    // �ӿڱ任������׼���ĵ�ת��Ϊ��Ļ����
    glViewport(0, 0, 800, 600);
    glEnable(GL_PROGRAM_POINT_SIZE); // ���õ��С

    // ע�ᴰ�ڴ�С�ı�ʱ�Ļص�����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //-----�����Ǵ��ڳ�ʼ��     -----
    //-----���濪ʼ����ͼ���׼��-----

    // 2.1����һ��������ɫ��
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER); // ���ض�����ɫ����������id��
    // ������ɫ��Դ��
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // ����Ϊ����ɫ��id���ַ�����������ɫ�����룬
    // ������ɫ��
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    // �����������������ɫ�������Ƿ�ɹ�
    {
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                 << infoLog << endl;
        }
    }

    // 2.2 ����Ƭ����ɫ��
    // �����붥����ɫ������
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // ����Ƭ����ɫ����������id��
    // ������ɫ��Դ��
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    // ������ɫ��
    glCompileShader(fragmentShader);
    // �����������������ɫ�������Ƿ�ɹ�
    {
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                 << infoLog << endl;
        }
    }

    // 2.3��ɫ������ �������ɫ���ϲ�
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram(); // ������ɫ������
    // �ϲ����㡢Ƭ����ɫ��
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // �����������������ɫ�������Ƿ�ɹ�
    {
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            cout << "ERROR::SHADER::shaderProgram::COMPILATION_FAILED\n"
                 << infoLog << endl;
        }
    }
    // ��glLinkProgram��������֮��ÿ����ɫ�����ú���Ⱦ���ö���ʹ��shaderProgram
    // ����vertexShader��fragmentShader�Ϳ���ɾ����
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 3��������
    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // ���Ͻ�
        0.5f, -0.5f, 0.0f,  // ���½�
        -0.5f, -0.5f, 0.0f, // ���½�
        -0.5f, 0.5f, 0.0f   // ���Ͻ�
    };

    unsigned int indices[] = {
        // ע��������0��ʼ!
        // ����������(0,1,2,3)���Ƕ�������vertices���±꣬
        // �����������±��������ϳɾ���

        0, 1, 3, // ��һ��������
        1, 2, 3  // �ڶ���������
    };

    // 3����VBO  VAO  EBO
    unsigned int VBO, VAO, EBO;
    glGenBuffers(1, &VBO);      // ���㻺�����
    glGenBuffers(1, &EBO);      // �����������
    glGenVertexArrays(1, &VAO); // �����������

    // 3.1 ��VAO
    glBindVertexArray(VAO); // ֮�����е� VBO �󶨡������������ã�������� VAO

    // 3.2 VBO��ʼ������VBO�󶨵�GL_ARRAY_BUFFER��
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // ����������vertices���嵽�ڴ���
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // GL_STATIC_DRAW��ʾ�������ݲ���ı�

    // 3.3 EBO��ʼ������VBO��ͬ���ǣ�����������ݵ��±�
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 3.4 ���ö������ԣ�glVertexAttribPointer������Ӧ����ν�����������
    // (1): 0��ʾ�������ݵ�λ��(location = 0)��(2): 3��ʾ��������������ֵvec3�� (3):���������ֵ��float��
    // (4): FALSE��ʾ�Ƿ�ϣ�����ݱ���׼��[-1,1]�� (5):����������������ռ�Ĵ�С�� (6): ������ʼλ�õ�ƫ������0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); // ���� location = 0 �Ķ�������(��������Ĭ�Ͻ���)
                                  // glBindVertexArray(0); // ��� VAO����ֹ�������޸�

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // �����߿����ģʽ
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // ���ģʽ

    // 4ѭ����Ⱦ
    while (!glfwWindowShouldClose(window)) // �����Ƿ�ر�
    {
        processInput(window); // ����Ƿ�������

        // ��Ⱦָ�� ....
        glClearColor(0.2f, 0.3f, 0.5f, 0.9f);
        glClear(GL_COLOR_BUFFER_BIT); // �ı���ɫ

        // ��������
        // Ϊʲô��ѭ���а�shaderprogram��VAO��
        // glUseProgram(shaderProgram);
        // glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 6); // ����������  0��ʾ��ʼ������6��ʾ��ĸ���
        // glDrawArrays(GL_POINTS, 0, 4);
        // glDrawArrays(GL_LINE_LOOP, 0, 4);
        //  glDrawArrays(GL_LINE_STRIP, 0, 4);

        // ʹ��EBO�Ļ����ͱ���ʹ��glDrawElements������
        // glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0); // GL_UNSIGNED_INT��ʾ����������  0��ʾƫ����
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window); // ������ɫ����
        glfwPollEvents();        // ����Ƿ����¼�
    }

    // ��Դ�ͷ�
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate(); // �ر�glfw
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // �ı��ӿڴ�С
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // ����Esc ���رմ���
        glfwSetWindowShouldClose(window, true);
}
