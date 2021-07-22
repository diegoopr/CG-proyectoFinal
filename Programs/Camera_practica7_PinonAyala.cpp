#include<stdio.h>
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include "Camera.h"
#include "Window.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
    position = startPosition;
    worldUp = startUp;
    yaw = startYaw;
    pitch = startPitch;
    front = glm::vec3(0.0f, 0.0f, -1.0f);

    moveSpeed = startMoveSpeed;
    turnSpeed = startTurnSpeed;

    update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime, GLboolean terceraPersona)
{
    GLfloat velocity = moveSpeed * deltaTime;

    if (keys[GLFW_KEY_W])
    {
        if(terceraPersona == true){
            if(position.y <=10 and position.y >=-5){
                position += front * velocity;
            }else{
                if(position.y >=10){
                    position.y = 9.9f;
                }
                if(position.y <=-5){
                    position.y = -4.9f;
                }
            }
        }else{
            position += front * velocity;
        }
    }

    if (keys[GLFW_KEY_S])
    {
        
        if(terceraPersona == true){
            if(position.y <=10 and position.y >=-5){
                position -= front * velocity;
            }else{
                if(position.y >=10){
                    position.y = 9.9f;
                }
                if(position.y <=-5){
                    position.y = -4.9f;
                }
            }
        }else{
            position -= front * velocity;
        }
    }

    if (keys[GLFW_KEY_A])
    {
        position -= right * velocity;
    }

    if (keys[GLFW_KEY_D])
    {
        position += right * velocity;
    }
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
    xChange *= turnSpeed;
    yChange *= turnSpeed;

    yaw += xChange;
    pitch += yChange;

    if (pitch > 89.0f)
    {
        pitch = 89.0f;
    }

    if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }

    update();
}

glm::mat4 Camera::calculateViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getCameraPosition()
{
    return position;
}


glm::vec3 Camera::getCameraDirection()
{
    return glm::normalize(front);
}

void Camera::update()
{
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}


Camera::~Camera()
{
}
