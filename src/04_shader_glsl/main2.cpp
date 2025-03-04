#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
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
                                 "layout (location = 1) in vec3 aColor;\n"
                                 "uniform vec2 xyOffset;\n"
                                 "out vec3 ourColor;\n"
                                 "void main()\n"
                                 "{\n"
                                 "    gl_Position = vec4(aPos.x + xyOffset.x, aPos.y + xyOffset.y, aPos.z, 1.0f);\n"
                                 "    ourColor = aColor;\n"
                                 "    gl_PointSize = 10.0f;\n"
                                 "}\n\0";
// Ƭ����ɫ��
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "in vec3 ourColor;\n" // �����������Ͷ���ͬvec3 ourColor
                                   "uniform vec3 colorOffset;\n"
                                   "void main()\n"
                                   "{\n"
                                   "    FragColor = vec4((ourColor + colorOffset)/2, 1.0f);\n"
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

    int count;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &count);
    cout << "GL_MAX_VERTEX_ATTRIBS = " << count << endl;

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
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 3��������
    float vertices[] = {
        // λ��              // ��ɫ
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // ����
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // ����
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // ����
    };

    // 3����VBO  VAO  EBO
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);      // ���㻺�����
    glGenVertexArrays(1, &VAO); // �����������

    // 3.1 ��VAO
    // ��һ��VAO VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3.4 ���ö���λ�����ԣ�   glVertexAttribPointer������Ӧ����ν�����������
    // (1): 0��ʾ�������ݵ�λ��(location = 0)��(2): 3��ʾ��ǰ�����ж��ٸ�ֵ�� (3):��ǰ��������ֵ��float���ͣ�
    // (4): FALSE��ʾ�Ƿ�ϣ�����ݱ���׼��[-1,1]�� (5):������������һ�������Ե�ֵ�ľ��룻 (6): ������ʼλ�õ�ƫ������0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); // ���� location = 0 �Ķ�������(��������Ĭ�Ͻ���)

    // 3.5���ö�����ɫ����
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // ���� location = 1 �Ķ�������(��������Ĭ�Ͻ���)

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // �����߿����ģʽ
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // ���ģʽ

    // 4ѭ����Ⱦ
    while (!glfwWindowShouldClose(window)) // �����Ƿ�ر�
    {
        processInput(window); // ����Ƿ�������

        // ��Ⱦָ�� ....
        glClearColor(0.2f, 0.3f, 0.5f, 0.9f);
        glClear(GL_COLOR_BUFFER_BIT); // �ı䱳����ɫ

        // ���Ƶ�һ��������
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        // ����fragment2��uniform
        float timeValue = glfwGetTime(); // ��ȡʱ��
        float Value1 = sin(timeValue) / 2;
        float Value2 = cos(timeValue) / 2;
        float Value3 = tan(timeValue) / 2;
        int xyOffsetLocation = glGetUniformLocation(shaderProgram, "xyOffset"); // ��ȡuniform vec2 xyOffset�ĵ�ַ
        // ����һ��uniform֮ǰ�������ʹ�ó��򣨵���glUseProgram)����ѯuniform��ַ��Ҫ��
        glUniform2f(xyOffsetLocation, Value1, Value2);

        int colorOffsetLocation = glGetUniformLocation(shaderProgram, "colorOffset");
        glUniform3f(colorOffsetLocation, abs(Value1) * 2, abs(Value2) * 2, abs(Value1) + abs(Value2));

        glDrawArrays(GL_TRIANGLES, 0, 3); // ����������
        glDrawArrays(GL_POINTS, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window); // ������ɫ����
        glfwPollEvents();        // ����Ƿ����¼�
    }

    // ��Դ�ͷ�
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
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
