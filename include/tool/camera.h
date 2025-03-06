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

// Ĭ��ֵ
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
    glm::vec3 Position; // ���λ��
    glm::vec3 Front;    // ��������
    glm::vec3 Up;       // ������
    glm::vec3 Right;    // ������
    glm::vec3 WorldUp;  // ����������

    float Yaw;                      // ƫ����
    float Pitch;                    // ������
    float MovementSpeed;            // �ƶ��ٶ�
    float Accelerate_MovementSpeed; // �����ٶ�
    float MouseSensitivity;         // �ӽ�������
    float Zoom;                     // ��Ұ��Χ  fov

    // ʹ��vec3�Ĺ��캯��
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

    // ʹ��float�Ĺ��캯��
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

    // ��ȡview����
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // ���̰����¼�
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

    // ����ƶ��¼�
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
        // ����view����
        updateCameraVectors();
    }

    // �������¼�
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // ���� Yaw �� Pitch����view����
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

    float Velocity; // �ٶ�  ��������
};
#endif