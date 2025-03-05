// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
// #include <tool/shader.h>
// #include <geometry/PlaneGeometry.h>

// #include <iostream>
// #include <math.h>

// #define STB_IMAGE_IMPLEMENTATION
// #include <tool/stb_image.h>

// using namespace std;

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

//     // ��glfw����   ���ڴ�С��������
//     GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
//     if (window == NULL)
//     {
//         cout << "failed to create GLFW window" << endl;
//         glfwTerminate();
//         return -1;
//     }
//     glfwMakeContextCurrent(window); // ����OpenGL��Ⱦ�ĸ�����

//     // ��ʼ��GLAD  glfwGetProcAddress��OpenGL����ָ���ַ�ĺ���
//     //  GLAD �����ѯ OpenGL �������򣬲���ȡ��ȷ�� API ������ַ
//     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//     {
//         cout << "Failed to initialize GLAD" << endl;
//         return -1;
//     }

//     // �����ӿڳߴ�  ǰ�������ƴ������½ǵ�λ�á�3��4��ʾ���ڵĿ�͸�
//     // �ӿڱ任������׼���ĵ�ת��Ϊ��Ļ����
//     glViewport(0, 0, 800, 600);
//     glEnable(GL_PROGRAM_POINT_SIZE);
//     glEnable(GL_BLEND);                                // ������ɫ���
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // �趨���ģʽ��ʵ�� ���� alpha ͸���ȵ���ɫ���

//     // ע�ᴰ�ڴ�С�ı�ʱ�Ļص�����
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

//     // ������ɫ��
//     Shader ourShader("./shader/vertex1.glsl", "./shader/fragment1.glsl");
//     // ����һ��ƽ�漸��ͼ��
//     PlaneGeometry planeGeometry(1.0, 1.0, 2, 2);

//     // ��������
//     unsigned int texture1, texture2; // ����ID
//     // �����һ������
//     glGenTextures(1, &texture1);            // ��������id
//     glBindTexture(GL_TEXTURE_2D, texture1); // ��������֮���κε�����ָ��������õ�ǰ�󶨵�����

//     // ���û��ƺ͹��˷�ʽ
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     // x��Ĺ��˷�ʽ   �ظ�
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     // Y��
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // ͼ����С   ���Թ���
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // ͼ��Ŵ�

//     // ͼ��y�ᷭת  ������λ��
//     stbi_set_flip_vertically_on_load(true);

//     // ����ͼƬ
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
//     glGenTextures(1, &texture2);                                      // ��������id
//     glBindTexture(GL_TEXTURE_2D, texture2);                           // ��������֮���κε�����ָ��������õ�ǰ�󶨵�����
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
//     ourShader.setInt("Texture1", 0);
//     // ourShader.setInt("Texture2", 1);

//     glActiveTexture(GL_TEXTURE0);           // ��������0  Ĭ�ϼ���0�����ֻ��һ������Ԫ�Ļ�
//     glBindTexture(GL_TEXTURE_2D, texture1); // ������0��texture1
//     glActiveTexture(GL_TEXTURE1);           // ��������1
//     glBindTexture(GL_TEXTURE_2D, texture2); // ������1��texture2

//     float angleValue = 0;
//     // ѭ����Ⱦ
//     while (!glfwWindowShouldClose(window)) // �����Ƿ�ر�
//     {
//         processInput(window); // ����Ƿ�������

//         // ��Ⱦָ�� ....
//         glClearColor(0.2f, 0.3f, 0.5f, 0.9f);
//         glClear(GL_COLOR_BUFFER_BIT); // �ı䱳����ɫ

//         angleValue = glfwGetTime();
//         ourShader.setFloat("angleValue", angleValue * 0.3);

//         glBindVertexArray(planeGeometry.VAO);
//         // glDrawElements(GL_POINTS, planeGeometry.indices.size(), GL_UNSIGNED_INT, 0);
//         // glDrawElements(GL_LINE_LOOP, planeGeometry.indices.size(), GL_UNSIGNED_INT, 0);
//         glDrawElements(GL_TRIANGLES, planeGeometry.indices.size(), GL_UNSIGNED_INT, 0);

//         glfwSwapBuffers(window); // ������ɫ����
//         glfwPollEvents();        // ����Ƿ����¼�
//     }
//     planeGeometry.dispose();
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
