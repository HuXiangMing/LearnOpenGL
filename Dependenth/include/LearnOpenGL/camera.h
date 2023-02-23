#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// ����ƶ��ķ������ڼ��̲ٿ�
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Ĭ���������
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  8.0f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


//һ�����������࣬���ڴ������벢������Ӧ��ŷ���ǡ������;��󣬹�OpenGLʹ��
class Camera
{
public:
    // ���ͶӰ����
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // ŷ����
    float Yaw;
    float Pitch;
    // ����ӽ����
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // �������Ĺ��캯��
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // ���б���ֵ�Ĺ��캯��
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // ����ʹ��ŷ���Ǻ�LookAt����������ͼ����
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }


    glm::mat4 My_GetViewMatrix()
    {
        return my_lookAt(Position, Position + Front, Up);
    }

    // �����������
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // ��������ƶ�
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // ������90��ʱ����Ļ����ַ�ת����Pitch��������
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // ����forword����
        updateCameraVectors();
    }

    //�����������¼�������
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // ����ŷ���������������forword����
    void updateCameraVectors()
    {
        // ����forword����
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // ���¼�����������᣺����ʹ��Front����ģ�Front���������Z���෴��������������ʱҲ�෴
        Right = glm::normalize(glm::cross(Front, WorldUp));  // ��������һ������Ϊ�����ϻ����¿���Խ�࣬���ǵĳ��Ⱦ�Խ�ӽ�0���Ӷ������ƶ��ٶȱ�����
        Up    = glm::normalize(glm::cross(Right, Front));
    }


    /// <summary>
    /// my_lookAt����
    /// </summary>
    /// <param name="position">�����λ��</param>
    /// <param name="target">�����Ŀ�귽��</param>
    /// <param name="worldUp">��������UP</param>
    /// <returns></returns>
    glm::mat4 my_lookAt(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
    {
        //�����������λ�ú͸�����ķ��򣬿����������Ŀ���ViewMatrix����ת����*ƽ�ƾ���

        //1.���������
        //�������Z�����ӿڷ����෴������ʹ��������ӿڵķ������
        glm::vec3 f = glm::normalize(target - position);
        glm::vec3 r = glm::normalize(glm::cross(f , glm::normalize(worldUp)));
        glm::vec3 u = glm::normalize(glm::cross(r, f));

        //2:��ת����
        //glm�ⴢ�����Ԫ�ز��õ��������ȵĴ��淽ʽ
        //����mat[i][j]��ʾ���ǵ� i �У��� j ��Ԫ��
        glm::mat4 rotation = glm::mat4(1.0f);
        rotation[0][0] = r.x;
        rotation[1][0] = r.y;
        rotation[2][0] = r.z;
        rotation[0][1] = u.x;
        rotation[1][1] = u.y;
        rotation[2][1] = u.z;
        rotation[0][2] = -f.x;
        rotation[1][2] = -f.y;
        rotation[2][2] = -f.z;

        //3��ƽ�ƾ���
        glm::mat4 translation = glm::mat4(1.0f);
        translation[3][0] = -position.x;
        translation[3][1] = -position.y;
        translation[3][2] = -position.z;

        return rotation * translation;
    }
};
#endif
