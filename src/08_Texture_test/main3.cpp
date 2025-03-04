// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
// #include <iostream>
// #include <math.h>
// #include <tool/shader.h>

// #define STB_IMAGE_IMPLEMENTATION
// #include <tool/stb_image.h>

// using namespace std;

// // ��ϰ���������ھ�����ֻ��ʾ����ͼ����м�һ���֣��޸��������꣬�ﵽ�ܿ������������ص�Ч����
// //        ����ʹ��GL_NEAREST��������˷�ʽ��������ʾ�ø�����

// /// @brief �ص��������û��ı䴰�ڵĴ�С��ʱ���ӿ�ҲӦ�ñ�����
// /// @param window  ����
// /// @param width_height ���ڸı��Ĵ�С
// void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// /// @brief �������
// /// @param window ���Ĵ���
// void processInput(GLFWwindow *window);

// string Shader::dirName;

// int main(int agrc, char *argv[])
// {
//     Shader::dirName = argv[1]; // argv[1] = "src/05_shader_class"
//     Shader::dirName += "/";

//     glfwInit();                                                    // ��ʼ��GLFW ,����Ҫ���ڴ���/�����ڣ������û������
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 // ��Ҫ�汾
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 // ��Ҫ�汾  ��3.3�汾��opengl
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // ����ģʽ

//     // 1.1����glfw����   ���ڴ�С��������
//     GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
//     if (window == NULL)
//     {
//         cout << "failed to create GLFW window" << endl;
//         glfwTerminate();
//         return -1;
//     }
//     glfwMakeContextCurrent(window); // ����OpenGL��Ⱦ�ĸ�����

//     // 1.2��ʼ��GLAD  glfwGetProcAddress��OpenGL����ָ���ַ�ĺ���
//     //  GLAD �����ѯ OpenGL �������򣬲���ȡ��ȷ�� API ������ַ
//     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//     {
//         cout << "Failed to initialize GLAD" << endl;
//         return -1;
//     }

//     // 1.3�����ӿڳߴ�  ǰ�������ƴ������½ǵ�λ�á�3��4��ʾ���ڵĿ�͸�
//     // �ӿڱ任������׼���ĵ�ת��Ϊ��Ļ����
//     glViewport(0, 0, 800, 600);

//     // ע�ᴰ�ڴ�С�ı�ʱ�Ļص�����
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

//     //-----�����Ǵ��ڳ�ʼ��     -----
//     //-----���濪ʼ����ͼ���׼��-----

//     // 2 ��������/Ƭ����ɫ��  ������ɫ������
//     Shader ourShader("./shader/vertex_test3.glsl", "./shader/fragment.glsl");
//     // 3��������
//     float vertices[] = {
//         //     ---- λ�� ----       ---- ��ɫ ----     - �������� -
//         0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // ����
//         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // ����
//         -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // ����
//         -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // ����
//     };

//     // ��������
//     unsigned int indices[] = {
//         0, 1, 3, // ������һ
//         1, 2, 3  // �����ζ�
//     };

//     // 3����VBO  VAO  EBO
//     unsigned int VBO, VAO, EBO;
//     glGenVertexArrays(1, &VAO); // �����������
//     glGenBuffers(1, &VBO);      // ���㻺�����
//     glGenBuffers(1, &EBO);      // ������������

//     // 3.1 ��VAO
//     // ��һ��VAO VBO
//     glBindVertexArray(VAO);
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     // ���EBO����
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

//     // 3.4 ���ö���λ�����ԣ�   glVertexAttribPointer������Ӧ����ν�����������
//     // (1): 0��ʾ�������ݵ�λ��(location = 0)��(2): 3��ʾ��ǰ�����ж��ٸ�ֵ�� (3):��ǰ��������ֵ��float���ͣ�
//     // (4): FALSE��ʾ�Ƿ�ϣ�����ݱ���׼��[-1,1]�� (5):������������һ�������Ե�ֵ�ľ��룻 (6): ������ʼλ�õ�ƫ������0
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
//     glEnableVertexAttribArray(0); // ���� location = 0 �Ķ�������(��������Ĭ�Ͻ���)

//     // 3.5���ö�����ɫ����
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
//     glEnableVertexAttribArray(1); // ���� location = 1 �Ķ�������(��������Ĭ�Ͻ���)

//     // 3.6���ö�����������
//     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
//     glEnableVertexAttribArray(2);

//     // 4 ��������
//     // 4.1��������
//     unsigned int texture1, texture2; // ����ID

//     // �����һ������
//     glGenTextures(1, &texture1);            // ��������id
//     glBindTexture(GL_TEXTURE_2D, texture1); // ��������֮���κε�����ָ��������õ�ǰ�󶨵�����

//     // 4.2���û��ƺ͹��˷�ʽ
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     // x��Ĺ��˷�ʽ   �ظ�
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     // Y��
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // ͼ����С   ���Թ���
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // ͼ��Ŵ�  GL_LINEAR  GL_NEAREST

//     // 4.3����ͼƬ
//     // ͼ��y�ᷭת  ������λ��
//     stbi_set_flip_vertically_on_load(true);
//     int width, height, nrChannels; // ��ɫͨ���ĸ���(3ͨ��rgb   4ͨ��rgba)
//     unsigned char *data = stbi_load("./static/texture/container.jpg", &width, &height, &nrChannels, 0);
//     if (data)
//     {
//         // ��һ��0ָ���༶��Զ������   �ڶ���0һ��Ϊ0(��ʷ��������)   GL_UNSIGNED_BYTE��ʾͼԴ�Ĵ洢��ʽ
//         // ��һ��RGB��ʾϣ��OPENGL������洢Ϊ���ָ�ʽ   �ڶ���PGB��ʾͼԴ�ĸ�ʽ
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D); // ʹ��opengl���ɶ༶��Զ����Mipmap��
//     }
//     // �����Ѿ����ɣ�ͼԴ�Ϳ����ͷ���
//     stbi_image_free(data);

//     // ����ڶ�������
//     glGenTextures(1, &texture2);            // ��������id
//     glBindTexture(GL_TEXTURE_2D, texture2); // ��������֮���κε�����ָ��������õ�ǰ�󶨵�����

//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     // x��Ļ��Ʒ�ʽ
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     // Y��Ļ��Ʒ�ʽ
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // ͼ����С�Ĺ���ģʽ �����Թ���  ����һ����GL_NEAREST  �ͽ�����
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // ͼ��Ŵ�

//     data = stbi_load("./static/texture/awesomeface.png", &width, &height, &nrChannels, 0);
//     if (data)
//     {
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D); // ʹ��opengl���ɶ༶��Զ����Mipmap��
//     }
//     stbi_image_free(data);

//     ourShader.use();
//     ourShader.setInt("Texture1", 0); // ����fragment�е�uniform
//     ourShader.setInt("Texture2", 1); // ����fragment�е�uniform

//     glActiveTexture(GL_TEXTURE0);           // ��������0  Ĭ�ϼ���0�����ֻ��һ������Ԫ�Ļ�
//     glBindTexture(GL_TEXTURE_2D, texture1); // ������0��texture1

//     glActiveTexture(GL_TEXTURE1);           // ��������1
//     glBindTexture(GL_TEXTURE_2D, texture2); // ������1��texture2

//     // 4ѭ����Ⱦ
//     while (!glfwWindowShouldClose(window)) // �����Ƿ�ر�
//     {
//         processInput(window); // ����Ƿ�������

//         // ��Ⱦָ�� ....
//         glClearColor(0.2f, 0.3f, 0.5f, 0.9f);
//         glClear(GL_COLOR_BUFFER_BIT); // �ı䱳����ɫ

//         glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

//         glfwSwapBuffers(window); // ������ɫ����
//         glfwPollEvents();        // ����Ƿ����¼�
//     }

//     // ��Դ�ͷ�
//     glDeleteVertexArrays(1, &VAO);
//     glDeleteBuffers(1, &VBO);
//     glDeleteBuffers(1, &EBO);

//     glfwTerminate(); // �ر�glfw
//     return 0;
// }

// void framebuffer_size_callback(GLFWwindow *window, int width, int height)
// {
//     // �ı��ӿڴ�С
//     glViewport(0, 0, width, height);
// }

// void processInput(GLFWwindow *window)
// {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // ����Esc ���رմ���
//         glfwSetWindowShouldClose(window, true);
// }
