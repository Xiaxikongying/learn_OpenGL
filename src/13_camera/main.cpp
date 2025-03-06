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

// /// @brief 回调函数当用户改变窗口的大小的时候，视口也应该被调整
// /// @param window  窗口
// /// @param width_height 窗口改变后的大小
// void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// /// @brief 检测输入
// /// @param window 检测的窗口
// void processInput(GLFWwindow *window);

// /// @brief 鼠标移动
// void mouse_callback(GLFWwindow *window, double xpos, double ypos);
// /// @brief 鼠标点击
// void mouse_button_calback(GLFWwindow *window, int button, int action, int mods);
// /// @brief 滚轮
// void scroll_callback(GLFWwindow *window, double x, double y);

// string Shader::dirName;

// // 相机位置  相机朝向  相机上方向
// glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
// glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// float deltaTime = 0.0f; // 当前帧与上一帧的时间差
// float lastFrame = 0.0f; // 上一帧的时间

// bool firstMouse = true;
// // 偏航角
// float yaw = -90.0f;
// // 俯仰角
// float pitch = 0.0f;
// float lastX = 800.0f / 2.0;
// float lastY = 600.0 / 2.0;
// float fov = 45.0f;
// float sensitivity = 0.05;

// int main(int agrc, char *argv[])
// {
//     Shader::dirName = argv[1]; // argv[1] = "src/05_shader_class"
//     Shader::dirName += "/";

//     glfwInit();                                                    // 初始化GLFW ,它主要用于创建/管理窗口，处理用户输入等
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 // 主要版本
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 // 次要版本  即3.3版本的opengl
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 核心模式

//     float screenWidth = 800;
//     float screenHeight = 600;
//     // 建glfw窗口   窗口大小，窗口名
//     GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
//     if (window == NULL)
//     {
//         cout << "failed to create GLFW window" << endl;
//         glfwTerminate();
//         return -1;
//     }
//     glfwMakeContextCurrent(window); // 告诉OpenGL渲染哪个窗口

//     // 初始化GLAD  glfwGetProcAddress是OpenGL函数指针地址的函数
//     //  GLAD 负责查询 OpenGL 驱动程序，并获取正确的 API 函数地址
//     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//     {
//         cout << "Failed to initialize GLAD" << endl;
//         return -1;
//     }

//     //--------------imgui设置--------------
//     // 创建GUI上下文
//     ImGui::CreateContext();
//     ImGuiIO &io = ImGui::GetIO();
//     (void)io;
//     // 设置imgui主题颜色
//     // ImGui::StyleColorsClassic(); // 经典皮肤
//     ImGui::StyleColorsDark(); // 暗色

//     // 设置平台
//     ImGui_ImplGlfw_InitForOpenGL(window, true);
//     const char *glsl_version = "#version 330";
//     ImGui_ImplOpenGL3_Init(glsl_version);
//     //--------------imgui设置完成--------------

//     // 设置视口尺寸  前两个控制窗口左下角的位置。3，4表示窗口的宽和高
//     // 视口变换，将标准化的点转换为屏幕坐标
//     glViewport(0, 0, 800, 600);
//     glEnable(GL_PROGRAM_POINT_SIZE);
//     glEnable(GL_BLEND);                                // 启用颜色混合
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 设定混合模式，实现 基于 alpha 透明度的颜色混合

//     glEnable(GL_DEPTH_TEST); // 开启深度测试
//     glDepthFunc(GL_LESS);

//     // 注册窗口大小改变时的回调函数
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//     // 2.鼠标事件
//     glfwSetCursorPosCallback(window, mouse_callback);
//     // glfwSetMouseButtonCallback(window, mouse_button_calback);
//     glfwSetScrollCallback(window, scroll_callback);

//     // 生成着色器
//     Shader ourShader("./shader/vertex.glsl", "./shader/fragment.glsl");
//     // 生成一个平面几何图形
//     BoxGeometry boxGeometry(1.0f, 1.0f, 1.0f);
//     SphereGeometry sphereGeometry(1.0f);
//     PlaneGeometry planeGeometry(1.0f, 1.0f);

//     // 生成纹理
//     unsigned int texture1, texture2; // 纹理ID
//     // 采入第一个纹理
//     glGenTextures(1, &texture1);            // 生成纹理id
//     glBindTexture(GL_TEXTURE_2D, texture1); // 绑定纹理，让之后任何的纹理指令都可以配置当前绑定的纹理

//     // 设置环绕和过滤方式
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     // x轴的过滤方式   重复
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     // Y轴
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 图像缩小   线性过滤
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 图像放大

//     // 图像y轴翻转  放任意位置
//     stbi_set_flip_vertically_on_load(true);

//     // 加载图片
//     int width, height, nrChannels; // 颜色通道的个数(3通道rgb   4通道rgba)
//     unsigned char *data = stbi_load("./static/texture/container.jpg", &width, &height, &nrChannels, 0);
//     if (data)
//     {
//         // 第一个0指定多级渐远纹理级别   第二个0一定为0(历史遗留问题)   GL_UNSIGNED_BYTE表示图源的存储格式
//         // 第一个RGB表示希望OPENGL将纹理存储为那种格式   第二个PGB表示图源的格式
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D); // 使用opengl生成多级渐远纹理（Mipmap）
//     }
//     // 纹理已经生成，图源就可以释放了
//     stbi_image_free(data);

//     // 采入第二个纹理
//     glGenTextures(1, &texture2);                                      // 生成纹理id
//     glBindTexture(GL_TEXTURE_2D, texture2);                           // 绑定纹理，让之后任何的纹理指令都可以配置当前绑定的纹理
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     // x轴的环绕方式
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     // Y轴的环绕方式
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 图像缩小的过滤模式 ：线性过滤  还有一种是GL_NEAREST  就近过滤
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 图像放大
//     data = stbi_load("./static/texture/awesomeface.png", &width, &height, &nrChannels, 0);
//     if (data)
//     {
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D); // 使用opengl生成多级渐远纹理（Mipmap）
//     }
//     stbi_image_free(data);

//     ourShader.use();
//     ourShader.setInt("Texture1", 0);
//     ourShader.setInt("Texture2", 1);

//     glActiveTexture(GL_TEXTURE0);           // 激活纹理0  默认激活0，如果只有一个纹理单元的话
//     glBindTexture(GL_TEXTURE_2D, texture1); // 让纹理0绑定texture1
//     glActiveTexture(GL_TEXTURE1);           // 激活纹理1
//     glBindTexture(GL_TEXTURE_2D, texture2); // 让纹理1绑定texture2

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

//     glm::vec3 clear_color = glm::vec3(0.2f, 0.3f, 0.5f); // 背景颜色

//     // 循环渲染
//     while (!glfwWindowShouldClose(window)) // 窗口是否关闭
//     {
//         processInput(window); // 检测是否有输入

//         // 创建imgui窗口
//         ImGui_ImplOpenGL3_NewFrame();
//         ImGui_ImplGlfw_NewFrame();
//         ImGui::NewFrame();

//         // 设置渲染内容
//         ImGui::Begin("ImGui");
//         // 显示帧率
//         ImGui::Text("%.3f ms-fram (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//         // 显示滑动条
//         ImGui::SliderFloat("fov", &fov, 1.0f, 60.0f);
//         ImGui::SliderFloat("aspect", &aspect, 0.5f, 2.0f);
//         ImGui::SliderFloat("sensitivity", &sensitivity, 0.01f, 0.1f);
//         ImGui::SliderFloat3("view", (float *)&cameraPos, -50.0f, 50.0f);
//         ImGui::End();

//         ImGui::Begin("ImGui2");
//         ImGui::ColorEdit3("clear color", (float *)&clear_color);
//         ImGui::End();

//         // 渲染指令 ....
//         // glClearColor(0.2f, 0.3f, 0.5f, 0.9f);
//         glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
//         // 清空颜色缓存  清空深度缓存
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//         value = glfwGetTime();
//         ourShader.setFloat("angleValue", value * 0.3);

//         deltaTime = value - lastFrame; // 计算与上一帧之间的时间差
//         lastFrame = value;             // 记录当前帧的时间

//         // view矩阵
//         glm::mat4 view = glm::mat4(1.0f);
//         // cameraPos.x = sin(value) * radius;
//         // cameraPos.z = cos(value) * radius;
//         // 三个参数为：相机位置坐标(eye)、目标点坐标(center) 和上向量(up)
//         // 通过它们可以计算出相机的 方向向量（朝向）、右向量
//         // 朝向向量 forward = center - eye
//         // 右向量 right = cross(up, forward)
//         view = glm::lookAt(cameraPos,
//                            cameraPos + cameraFront,
//                            cameraUp);
//         ourShader.setMat4("view", view);

//         // projection矩阵 fov 宽高比 近平面  远平面
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

//         // 画球体
//         model = glm::mat4(1.0f);
//         model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
//         ourShader.setMat4("model", model);
//         glBindVertexArray(sphereGeometry.VAO);
//         // glDrawElements(GL_POINTS, sphereGeometry.indices.size(), GL_UNSIGNED_INT, 0);
//         glDrawElements(GL_LINE_LOOP, sphereGeometry.indices.size(), GL_UNSIGNED_INT, 0);

//         // 画平面
//         model = glm::mat4(1.0f);
//         model = glm::translate(model, glm::vec3(-4.0f, 0.0f, 0.0f));
//         ourShader.setMat4("model", model);
//         glBindVertexArray(planeGeometry.VAO);
//         glDrawElements(GL_TRIANGLES, planeGeometry.indices.size(), GL_UNSIGNED_INT, 0);

//         // 开始渲染imgui  写在图形后面，才不会被图形遮挡
//         ImGui::Render();
//         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

//         glfwSwapBuffers(window); // 交换颜色缓冲
//         glfwPollEvents();        // 检测是否有事件
//     }
//     boxGeometry.dispose();
//     planeGeometry.dispose();
//     sphereGeometry.dispose();
//     glfwTerminate(); // 关闭glfw
//     return 0;
// }

// void framebuffer_size_callback(GLFWwindow *window, int width, int height)
// {
//     // 改变视口大小
//     glViewport(0, 0, width, height);
// }

// void processInput(GLFWwindow *window)
// {
//     float cameraSpeed = 2.5f;
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // 输入Esc ，关闭窗口
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

//     // 鼠标xy轴的偏移量
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