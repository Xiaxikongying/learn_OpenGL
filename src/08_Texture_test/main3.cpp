// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
// #include <iostream>
// #include <math.h>
// #include <tool/shader.h>

// #define STB_IMAGE_IMPLEMENTATION
// #include <tool/stb_image.h>

// using namespace std;

// // 练习三：尝试在矩形上只显示纹理图像的中间一部分，修改纹理坐标，达到能看见单个的像素的效果。
// //        尝试使用GL_NEAREST的纹理过滤方式让像素显示得更清晰

// /// @brief 回调函数当用户改变窗口的大小的时候，视口也应该被调整
// /// @param window  窗口
// /// @param width_height 窗口改变后的大小
// void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// /// @brief 检测输入
// /// @param window 检测的窗口
// void processInput(GLFWwindow *window);

// string Shader::dirName;

// int main(int agrc, char *argv[])
// {
//     Shader::dirName = argv[1]; // argv[1] = "src/05_shader_class"
//     Shader::dirName += "/";

//     glfwInit();                                                    // 初始化GLFW ,它主要用于创建/管理窗口，处理用户输入等
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 // 主要版本
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 // 次要版本  即3.3版本的opengl
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 核心模式

//     // 1.1创建glfw窗口   窗口大小，窗口名
//     GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
//     if (window == NULL)
//     {
//         cout << "failed to create GLFW window" << endl;
//         glfwTerminate();
//         return -1;
//     }
//     glfwMakeContextCurrent(window); // 告诉OpenGL渲染哪个窗口

//     // 1.2初始化GLAD  glfwGetProcAddress是OpenGL函数指针地址的函数
//     //  GLAD 负责查询 OpenGL 驱动程序，并获取正确的 API 函数地址
//     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//     {
//         cout << "Failed to initialize GLAD" << endl;
//         return -1;
//     }

//     // 1.3设置视口尺寸  前两个控制窗口左下角的位置。3，4表示窗口的宽和高
//     // 视口变换，将标准化的点转换为屏幕坐标
//     glViewport(0, 0, 800, 600);

//     // 注册窗口大小改变时的回调函数
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

//     //-----上面是窗口初始化     -----
//     //-----下面开始绘制图像的准备-----

//     // 2 创建顶点/片段着色器  创建着色器程序
//     Shader ourShader("./shader/vertex_test3.glsl", "./shader/fragment.glsl");
//     // 3顶点数据
//     float vertices[] = {
//         //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
//         0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // 右上
//         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 右下
//         -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 左下
//         -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // 左上
//     };

//     // 索引数据
//     unsigned int indices[] = {
//         0, 1, 3, // 三角形一
//         1, 2, 3  // 三角形二
//     };

//     // 3创建VBO  VAO  EBO
//     unsigned int VBO, VAO, EBO;
//     glGenVertexArrays(1, &VAO); // 顶点数组对象
//     glGenBuffers(1, &VBO);      // 顶点缓冲对象
//     glGenBuffers(1, &EBO);      // 顶点索引对象

//     // 3.1 绑定VAO
//     // 第一个VAO VBO
//     glBindVertexArray(VAO);
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     // 填充EBO数据
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

//     // 3.4 设置顶点位置属性，   glVertexAttribPointer：设置应该如何解析顶点数据
//     // (1): 0表示顶点数据的位置(location = 0)；(2): 3表示当前属性有多少个值； (3):当前属性三个值是float类型；
//     // (4): FALSE表示是否希望数据被标准化[-1,1]； (5):步长：距离下一个该属性的值的距离； (6): 数据起始位置的偏移量：0
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
//     glEnableVertexAttribArray(0); // 启用 location = 0 的顶点属性(顶点属性默认禁用)

//     // 3.5设置顶点颜色属性
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
//     glEnableVertexAttribArray(1); // 启用 location = 1 的顶点属性(顶点属性默认禁用)

//     // 3.6设置顶点纹理属性
//     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
//     glEnableVertexAttribArray(2);

//     // 4 纹理生成
//     // 4.1生成纹理
//     unsigned int texture1, texture2; // 纹理ID

//     // 采入第一个纹理
//     glGenTextures(1, &texture1);            // 生成纹理id
//     glBindTexture(GL_TEXTURE_2D, texture1); // 绑定纹理，让之后任何的纹理指令都可以配置当前绑定的纹理

//     // 4.2设置环绕和过滤方式
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     // x轴的过滤方式   重复
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     // Y轴
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 图像缩小   线性过滤
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 图像放大  GL_LINEAR  GL_NEAREST

//     // 4.3加载图片
//     // 图像y轴翻转  放任意位置
//     stbi_set_flip_vertically_on_load(true);
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
//     glGenTextures(1, &texture2);            // 生成纹理id
//     glBindTexture(GL_TEXTURE_2D, texture2); // 绑定纹理，让之后任何的纹理指令都可以配置当前绑定的纹理

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
//     ourShader.setInt("Texture1", 0); // 设置fragment中的uniform
//     ourShader.setInt("Texture2", 1); // 设置fragment中的uniform

//     glActiveTexture(GL_TEXTURE0);           // 激活纹理0  默认激活0，如果只有一个纹理单元的话
//     glBindTexture(GL_TEXTURE_2D, texture1); // 让纹理0绑定texture1

//     glActiveTexture(GL_TEXTURE1);           // 激活纹理1
//     glBindTexture(GL_TEXTURE_2D, texture2); // 让纹理1绑定texture2

//     // 4循环渲染
//     while (!glfwWindowShouldClose(window)) // 窗口是否关闭
//     {
//         processInput(window); // 检测是否有输入

//         // 渲染指令 ....
//         glClearColor(0.2f, 0.3f, 0.5f, 0.9f);
//         glClear(GL_COLOR_BUFFER_BIT); // 改变背景颜色

//         glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

//         glfwSwapBuffers(window); // 交换颜色缓冲
//         glfwPollEvents();        // 检测是否有事件
//     }

//     // 资源释放
//     glDeleteVertexArrays(1, &VAO);
//     glDeleteBuffers(1, &VBO);
//     glDeleteBuffers(1, &EBO);

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
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // 输入Esc ，关闭窗口
//         glfwSetWindowShouldClose(window, true);
// }
