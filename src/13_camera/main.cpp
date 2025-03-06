// #include <glad/glad.h>
// #include <GLFW/glfw3.h>

// #include <tool/shader.h>
// #include <tool/gui.h>

// #include <geometry/PlaneGeometry.h>
// #include <geometry/BoxGeometry.h>
// #include <geometry/SphereGeometry.h>

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

// /// @brief ����ƶ�
// void mouse_callback(GLFWwindow *window, double xpos, double ypos);
// /// @brief �����
// void mouse_button_calback(GLFWwindow *window, int button, int action, int mods);
// /// @brief ����
// void scroll_callback(GLFWwindow *window, double x, double y);

// string Shader::dirName;

// // ���λ��  �������  ����Ϸ���
// glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
// glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
// float lastFrame = 0.0f; // ��һ֡��ʱ��

// bool firstMouse = true;
// // ƫ����
// float yaw = -90.0f;
// // ������
// float pitch = 0.0f;
// float lastX = 800.0f / 2.0;
// float lastY = 600.0 / 2.0;
// float fov = 45.0f;
// float sensitivity = 0.05;

// int main(int agrc, char *argv[])
// {
//     Shader::dirName = argv[1]; // argv[1] = "src/05_shader_class"
//     Shader::dirName += "/";

//     glfwInit();                                                    // ��ʼ��GLFW ,����Ҫ���ڴ���/�����ڣ������û������
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 // ��Ҫ�汾
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 // ��Ҫ�汾  ��3.3�汾��opengl
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // ����ģʽ

//     float screenWidth = 800;
//     float screenHeight = 600;
//     // ��glfw����   ���ڴ�С��������
//     GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
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

//     //--------------imgui����--------------
//     // ����GUI������
//     ImGui::CreateContext();
//     ImGuiIO &io = ImGui::GetIO();
//     (void)io;
//     // ����imgui������ɫ
//     // ImGui::StyleColorsClassic(); // ����Ƥ��
//     ImGui::StyleColorsDark(); // ��ɫ

//     // ����ƽ̨
//     ImGui_ImplGlfw_InitForOpenGL(window, true);
//     const char *glsl_version = "#version 330";
//     ImGui_ImplOpenGL3_Init(glsl_version);
//     //--------------imgui�������--------------

//     // �����ӿڳߴ�  ǰ�������ƴ������½ǵ�λ�á�3��4��ʾ���ڵĿ�͸�
//     // �ӿڱ任������׼���ĵ�ת��Ϊ��Ļ����
//     glViewport(0, 0, 800, 600);
//     glEnable(GL_PROGRAM_POINT_SIZE);
//     glEnable(GL_BLEND);                                // ������ɫ���
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // �趨���ģʽ��ʵ�� ���� alpha ͸���ȵ���ɫ���

//     glEnable(GL_DEPTH_TEST); // ������Ȳ���
//     glDepthFunc(GL_LESS);

//     // ע�ᴰ�ڴ�С�ı�ʱ�Ļص�����
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//     // 2.����¼�
//     glfwSetCursorPosCallback(window, mouse_callback);
//     // glfwSetMouseButtonCallback(window, mouse_button_calback);
//     glfwSetScrollCallback(window, scroll_callback);

//     // ������ɫ��
//     Shader ourShader("./shader/vertex.glsl", "./shader/fragment.glsl");
//     // ����һ��ƽ�漸��ͼ��
//     BoxGeometry boxGeometry(1.0f, 1.0f, 1.0f);
//     SphereGeometry sphereGeometry(1.0f);
//     PlaneGeometry planeGeometry(1.0f, 1.0f);

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
//     ourShader.setInt("Texture2", 1);

//     glActiveTexture(GL_TEXTURE0);           // ��������0  Ĭ�ϼ���0�����ֻ��һ������Ԫ�Ļ�
//     glBindTexture(GL_TEXTURE_2D, texture1); // ������0��texture1
//     glActiveTexture(GL_TEXTURE1);           // ��������1
//     glBindTexture(GL_TEXTURE_2D, texture2); // ������1��texture2

//     glm::vec3 cubePositions[] =
//         {
//             glm::vec3(0.0f, 0.0f, 0.0f),
//             glm::vec3(2.0f, 5.0f, -15.0f),
//             glm::vec3(-1.5f, -2.2f, -2.5f),
//             glm::vec3(-3.8f, -2.0f, -12.3f),
//             glm::vec3(2.4f, -0.4f, -3.5f),
//             glm::vec3(-1.7f, 3.0f, -7.5f),
//             glm::vec3(1.3f, -2.0f, -2.5f),
//             glm::vec3(1.5f, 2.0f, -2.5f),
//             glm::vec3(1.5f, 0.2f, -1.5f),
//             glm::vec3(-1.3f, 1.0f, -1.5f)};

//     float value = 0;
//     float radius = 10.0f;
//     float aspect = screenWidth / screenHeight;

//     glm::vec3 clear_color = glm::vec3(0.2f, 0.3f, 0.5f); // ������ɫ

//     // ѭ����Ⱦ
//     while (!glfwWindowShouldClose(window)) // �����Ƿ�ر�
//     {
//         processInput(window); // ����Ƿ�������

//         // ����imgui����
//         ImGui_ImplOpenGL3_NewFrame();
//         ImGui_ImplGlfw_NewFrame();
//         ImGui::NewFrame();

//         // ������Ⱦ����
//         ImGui::Begin("ImGui");
//         // ��ʾ֡��
//         ImGui::Text("%.3f ms-fram (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//         // ��ʾ������
//         ImGui::SliderFloat("fov", &fov, 1.0f, 60.0f);
//         ImGui::SliderFloat("aspect", &aspect, 0.5f, 2.0f);
//         ImGui::SliderFloat("sensitivity", &sensitivity, 0.01f, 0.1f);
//         ImGui::SliderFloat3("view", (float *)&cameraPos, -50.0f, 50.0f);
//         ImGui::End();

//         ImGui::Begin("ImGui2");
//         ImGui::ColorEdit3("clear color", (float *)&clear_color);
//         ImGui::End();

//         // ��Ⱦָ�� ....
//         // glClearColor(0.2f, 0.3f, 0.5f, 0.9f);
//         glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
//         // �����ɫ����  �����Ȼ���
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//         value = glfwGetTime();
//         ourShader.setFloat("angleValue", value * 0.3);

//         deltaTime = value - lastFrame; // ��������һ֮֡���ʱ���
//         lastFrame = value;             // ��¼��ǰ֡��ʱ��

//         // view����
//         glm::mat4 view = glm::mat4(1.0f);
//         // cameraPos.x = sin(value) * radius;
//         // cameraPos.z = cos(value) * radius;
//         // ��������Ϊ�����λ������(eye)��Ŀ�������(center) ��������(up)
//         // ͨ�����ǿ��Լ��������� �������������򣩡�������
//         // �������� forward = center - eye
//         // ������ right = cross(up, forward)
//         view = glm::lookAt(cameraPos,
//                            cameraPos + cameraFront,
//                            cameraUp);
//         ourShader.setMat4("view", view);

//         // projection���� fov ��߱� ��ƽ��  Զƽ��
//         glm::mat4 projection = glm::mat4(1.0f);
//         projection = glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);
//         ourShader.setMat4("projection", projection);

//         glm::mat4 model = glm::mat4(1.0f);
//         glBindVertexArray(boxGeometry.VAO);
//         for (int i = 0; i < 10; ++i)
//         {
//             model = glm::mat4(1.0f);
//             model = glm::translate(model, cubePositions[i]);
//             if (i % 3 == 0)
//                 model = glm::rotate(model, value * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
//             ourShader.setMat4("model", model);

//             glDrawElements(GL_TRIANGLES, boxGeometry.indices.size(), GL_UNSIGNED_INT, 0);
//         }

//         // ������
//         model = glm::mat4(1.0f);
//         model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
//         ourShader.setMat4("model", model);
//         glBindVertexArray(sphereGeometry.VAO);
//         // glDrawElements(GL_POINTS, sphereGeometry.indices.size(), GL_UNSIGNED_INT, 0);
//         glDrawElements(GL_LINE_LOOP, sphereGeometry.indices.size(), GL_UNSIGNED_INT, 0);

//         // ��ƽ��
//         model = glm::mat4(1.0f);
//         model = glm::translate(model, glm::vec3(-4.0f, 0.0f, 0.0f));
//         ourShader.setMat4("model", model);
//         glBindVertexArray(planeGeometry.VAO);
//         glDrawElements(GL_TRIANGLES, planeGeometry.indices.size(), GL_UNSIGNED_INT, 0);

//         // ��ʼ��Ⱦimgui  д��ͼ�κ��棬�Ų��ᱻͼ���ڵ�
//         ImGui::Render();
//         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

//         glfwSwapBuffers(window); // ������ɫ����
//         glfwPollEvents();        // ����Ƿ����¼�
//     }
//     boxGeometry.dispose();
//     planeGeometry.dispose();
//     sphereGeometry.dispose();
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
//     float cameraSpeed = 2.5f;
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // ����Esc ���رմ���
//         glfwSetWindowShouldClose(window, true);
//     if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
//         cameraSpeed = 7.5f * deltaTime;
//     else
//         cameraSpeed = 2.5f * deltaTime;

//     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//         cameraPos += cameraSpeed * cameraFront;
//     if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//         cameraPos -= cameraSpeed * cameraFront;
//     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//         cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
//     if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//         cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
// }

// void mouse_callback(GLFWwindow *window, double xpos, double ypos)
// {
//     cout << "xpos = " << xpos << "   ypos = " << ypos << endl;
//     if (firstMouse)
//     {
//         lastX = xpos;
//         lastY = ypos;
//         firstMouse = false;
//     }

//     // ���xy���ƫ����
//     float xoffset = xpos - lastX;
//     float yoffset = lastY - ypos;
//     lastX = xpos;
//     lastY = ypos;

//     xoffset *= sensitivity;
//     yoffset *= sensitivity;

//     yaw += xoffset;
//     pitch += yoffset;

//     if (pitch > 89.0f)
//         pitch = 89.0f;
//     if (pitch < -89.0f)
//         pitch = -89.0f;

//     glm::vec3 front;
//     front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//     front.y = sin(glm::radians(pitch));
//     front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//     cameraFront = glm::normalize(front);
// }
// void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
// {
//     fov -= (float)yoffset;
//     if (fov < 1.0f)
//         fov = 1.0f;
//     if (fov > 60.0f)
//         fov = 60.0f;
// }