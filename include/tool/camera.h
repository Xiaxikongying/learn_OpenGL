#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
using namespace std;

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement : int
{
    NONE = 0,
    FORWARD = 1,
    BACKWARD = 2,
    LEFT = 4,
    RIGHT = 8,
    ACCELERATE = 16,
};

inline Camera_Movement operator|(Camera_Movement lhs, Camera_Movement rhs)
{
    return static_cast<Camera_Movement>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

// 默认值
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
    glm::vec3 Position; // 相机位置
    glm::vec3 Front;    // 朝向向量
    glm::vec3 Up;       // 上向量
    glm::vec3 Right;    // 右向量
    glm::vec3 WorldUp;  // 世界上向量

    float Yaw;                      // 偏航角
    float Pitch;                    // 俯仰角
    float MovementSpeed;            // 移动速度
    float Accelerate_MovementSpeed; // 加速速度
    float MouseSensitivity;         // 视角灵敏度
    float Zoom;                     // 视野范围  fov

    // 使用vec3的构造函数
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW, float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
          MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        Accelerate_MovementSpeed = MovementSpeed * 3;
        updateCameraVectors();
    }

    // 使用float的构造函数
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        Accelerate_MovementSpeed = MovementSpeed * 3;
        updateCameraVectors();
    }

    // 获取view矩阵
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // 键盘按键事件
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        if (direction & ACCELERATE)
            Velocity = Accelerate_MovementSpeed * deltaTime;
        else
            Velocity = MovementSpeed * deltaTime;

        if (direction & FORWARD)
            Position += Front * Velocity;
        if (direction & BACKWARD)
            Position -= Front * Velocity;
        if (direction & LEFT)
            Position -= Right * Velocity;
        if (direction & RIGHT)
            Position += Right * Velocity;
    }

    // 鼠标移动事件
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }
        // 更新view矩阵
        updateCameraVectors();
    }

    // 鼠标滚轮事件
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // 根据 Yaw 和 Pitch更新view矩阵
    void updateCameraVectors()
    {
        glm::vec3 front = glm::vec3(1.0f);
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

    float Velocity; // 速度  不可配置
};
#endif