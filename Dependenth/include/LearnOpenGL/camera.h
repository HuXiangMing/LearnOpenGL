#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// 相机移动的方向，用于键盘操控
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// 默认相机参数
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  8.0f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


//一个抽象的相机类，用于处理输入并计算相应的欧拉角、向量和矩阵，供OpenGL使用
class Camera
{
public:
    // 相机投影参数
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // 欧拉角
    float Yaw;
    float Pitch;
    // 相机视角相关
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // 带向量的构造函数
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // 带有标量值的构造函数
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // 返回使用欧拉角和LookAt矩阵计算的视图矩阵
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }


    glm::mat4 My_GetViewMatrix()
    {
        return my_lookAt(Position, Position + Front, Up);
    }

    // 处理键盘输入
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

    // 处理鼠标移动
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // 当超出90°时，屏幕会出现翻转，对Pitch角做限制
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // 更新forword向量
        updateCameraVectors();
    }

    //处理鼠标滚轮事件的输入
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // 根据欧拉角来计算相机的forword向量
    void updateCameraVectors()
    {
        // 计算forword向量
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // 重新计算右轴和上轴：这里使用Front计算的，Front与摄像机的Z轴相反，所以在下面点乘时也相反
        Right = glm::normalize(glm::cross(Front, WorldUp));  // 将向量归一化，因为你向上或向下看得越多，它们的长度就越接近0，从而导致移动速度变慢。
        Up    = glm::normalize(glm::cross(Right, Front));
    }


    /// <summary>
    /// my_lookAt函数
    /// </summary>
    /// <param name="position">摄像机位置</param>
    /// <param name="target">看向的目标方向</param>
    /// <param name="worldUp">世界坐标UP</param>
    /// <returns></returns>
    glm::mat4 my_lookAt(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
    {
        //根据摄像机的位置和各个轴的方向，可以算出朝向目标的ViewMatrix（旋转矩阵*平移矩阵）

        //1.摄像机方向
        //摄像机的Z轴与视口方向相反，我们使用摄像机视口的方向计算
        glm::vec3 f = glm::normalize(target - position);
        glm::vec3 r = glm::normalize(glm::cross(f , glm::normalize(worldUp)));
        glm::vec3 u = glm::normalize(glm::cross(r, f));

        //2:旋转矩阵
        //glm库储存矩阵元素采用的是列优先的储存方式
        //所以mat[i][j]表示的是第 i 列，第 j 行元素
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

        //3：平移矩阵
        glm::mat4 translation = glm::mat4(1.0f);
        translation[3][0] = -position.x;
        translation[3][1] = -position.y;
        translation[3][2] = -position.z;

        return rotation * translation;
    }
};
#endif
