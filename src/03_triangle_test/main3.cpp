#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;

// ��ϰ3�� ����������ɫ��������������������ɫ��ͬ

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
                                 "gl_PointSize = 10.0f;\n"
                                 "}\n\0";
// Ƭ����ɫ��
const char *fragmentShaderSource1 = "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "void main()\n"
                                    "{\n"
                                    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                    "}\n\0";

// Ƭ����ɫ��
const char *fragmentShaderSource2 = "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "void main()\n"
                                    "{\n"
                                    "   FragColor = vec4(0.2f, 0.7f, 0.8f, 1.0f);\n"
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
    unsigned int fragmentShaders[2];
    fragmentShaders[0] = glCreateShader(GL_FRAGMENT_SHADER); // ����Ƭ����ɫ����������id��
    // ������ɫ��Դ��
    glShaderSource(fragmentShaders[0], 1, &fragmentShaderSource1, NULL);
    // ������ɫ��
    glCompileShader(fragmentShaders[0]);
    // �����������������ɫ�������Ƿ�ɹ�
    {
        glGetShaderiv(fragmentShaders[0], GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShaders[0], 512, NULL, infoLog);
            cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                 << infoLog << endl;
        }
    }

    fragmentShaders[1] = glCreateShader(GL_FRAGMENT_SHADER); // ����Ƭ����ɫ����������id��
    // ������ɫ��Դ��
    glShaderSource(fragmentShaders[1], 1, &fragmentShaderSource2, NULL);
    // ������ɫ��
    glCompileShader(fragmentShaders[1]);
    // �����������������ɫ�������Ƿ�ɹ�
    {
        glGetShaderiv(fragmentShaders[1], GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShaders[1], 512, NULL, infoLog);
            cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                 << infoLog << endl;
        }
    }

    // 2.3��ɫ������ �������ɫ���ϲ�
    unsigned int shaderPrograms[2];
    shaderPrograms[0] = glCreateProgram(); // ������ɫ������
    // �ϲ����㡢Ƭ����ɫ��
    glAttachShader(shaderPrograms[0], vertexShader);
    glAttachShader(shaderPrograms[0], fragmentShaders[0]);
    glLinkProgram(shaderPrograms[0]);
    // �����������������ɫ�������Ƿ�ɹ�
    {
        glGetProgramiv(shaderPrograms[0], GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shaderPrograms[0], 512, NULL, infoLog);
            cout << "ERROR::SHADER::shaderProgram::COMPILATION_FAILED\n"
                 << infoLog << endl;
        }
    }

    shaderPrograms[1] = glCreateProgram(); // ������ɫ������
    // �ϲ����㡢Ƭ����ɫ��
    glAttachShader(shaderPrograms[1], vertexShader);
    glAttachShader(shaderPrograms[1], fragmentShaders[1]);
    glLinkProgram(shaderPrograms[1]);
    // �����������������ɫ�������Ƿ�ɹ�
    {
        glGetProgramiv(shaderPrograms[1], GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shaderPrograms[1], 512, NULL, infoLog);
            cout << "ERROR::SHADER::shaderProgram::COMPILATION_FAILED\n"
                 << infoLog << endl;
        }
    }

    // ��glLinkProgram��������֮��ÿ����ɫ�����ú���Ⱦ���ö���ʹ��shaderProgram
    // ����vertexShader��fragmentShader�Ϳ���ɾ����
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShaders[0]);
    glDeleteShader(fragmentShaders[1]);

    // 3��������
    float vertices[] = {
        -0.9f, -0.5f, 0.0f, // left
        -0.0f, -0.5f, 0.0f, // right
        -0.45f, 0.5f, 0.0f, // top

        0.0f, -0.5f, 0.0f, // left
        0.9f, -0.5f, 0.0f, // right
        0.45f, 0.5f, 0.0f  // top
    };

    // 3����VBO  VAO  EBO
    unsigned int VBOS[2], VAOS[2];
    glGenBuffers(2, VBOS);      // ���㻺�����
    glGenVertexArrays(2, VAOS); // �����������

    // 3.1 ��VAO
    // ��һ��VAO VBO
    glBindVertexArray(VAOS[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOS[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3.4 ���ö������ԣ�glVertexAttribPointer������Ӧ����ν�����������
    // (1): 0��ʾ�������ݵ�λ��(location = 0)��(2): 3��ʾ��������������ֵvec3�� (3):���������ֵ��float��
    // (4): FALSE��ʾ�Ƿ�ϣ�����ݱ���׼��[-1,1]�� (5):����������������ռ�Ĵ�С�� (6): ������ʼλ�õ�ƫ������0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); // ���� location = 0 �Ķ�������(��������Ĭ�Ͻ���)

    // �ڶ���VAO VBO
    glBindVertexArray(VAOS[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOS[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(9 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // �����߿����ģʽ
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // ���ģʽ

    // 4ѭ����Ⱦ
    while (!glfwWindowShouldClose(window)) // �����Ƿ�ر�
    {
        processInput(window); // ����Ƿ�������

        // ��Ⱦָ�� ....
        glClearColor(0.2f, 0.3f, 0.5f, 0.9f);
        glClear(GL_COLOR_BUFFER_BIT); // �ı䱳����ɫ

        glUseProgram(shaderPrograms[0]);
        glBindVertexArray(VAOS[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3); // ����������
        glDrawArrays(GL_POINTS, 0, 3);

        glUseProgram(shaderPrograms[1]);
        glBindVertexArray(VAOS[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3); // ����������
        glDrawArrays(GL_POINTS, 0, 3);

        glfwSwapBuffers(window); // ������ɫ����
        glfwPollEvents();        // ����Ƿ����¼�
    }

    // ��Դ�ͷ�
    glDeleteVertexArrays(2, VAOS);
    glDeleteBuffers(2, VBOS);
    glDeleteProgram(shaderPrograms[0]);
    glDeleteProgram(shaderPrograms[1]);

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
