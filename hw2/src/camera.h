#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <vector>

// #include <glm/gtx/string_cast.hpp>
// #include <iostream>


namespace engine
{
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

enum CameraMode
{
    PERSPECTIVE,
    ORTHOGONAL
};

// Default camera values
const float SPEED       =   2.5f;

const float YAW         =   0.0f;
const float PITCH       =   0.0f;
const float ROLL        =   0.0f;
const float ROT_SENSITIVITY = 0.1f;

const float NEAR        =   0.1f;
const float FAR         = 100.0f;

const float ZOOM        =  45.0f; // (FOV)
const float MAX_ZOOM    = 135.0f;
const float MIN_ZOOM    =  10.0f;
const float ZOOM_SENSITIVITY = 1.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 position;
    glm::vec3 rotation; // Euler angles (Pitch, Yaw, Roll)
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    glm::vec3 cameraRight;
    glm::vec3 worldUp;

    // Camera options
    float movementSpeed;
    float rotationSensitivity;
    float zoomSensitivity;
    float zoom;
    CameraMode mode;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float roll = ROLL, CameraMode mode = CameraMode::PERSPECTIVE)
        : cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), rotationSensitivity(ROT_SENSITIVITY), zoom(ZOOM), zoomSensitivity(ZOOM_SENSITIVITY)
    {
        this->position = position;
        this->cameraUp = up;
        this->cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
        this->rotation = glm::vec3(pitch, yaw, roll);
        this->mode = mode;
    }

    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float roll = ROLL)
        : cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), rotationSensitivity(ROT_SENSITIVITY), zoom(ZOOM), zoomSensitivity(ZOOM_SENSITIVITY)
    {
        this->position = glm::vec3(posX, posY, posZ);
        this->cameraUp = glm::vec3(upX, upY, upZ);
        this->cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
        this->rotation = glm::vec3(pitch, yaw, roll);
        this->mode = mode;
    }

    // Returns the view matrix calculated using Euler angles and the LookAt matrix
    glm::mat4 getViewMatrix()
    {
        return glm::lookAt(this->position, this->position + this->cameraFront, this->cameraUp);
    }

    // Returns the projection matrix calculated using field of view and screen size
    glm::mat4 getProjectionMatrix(unsigned int scrWidth, unsigned int scrHeight)
    {
        switch (this->mode)
        {
        case CameraMode::PERSPECTIVE:
            return glm::perspective(
                glm::radians(this->zoom),
                (float)scrWidth / scrHeight,
                NEAR,
                FAR
            );
        case CameraMode::ORTHOGONAL:
            return glm::ortho(0.0f, (float)scrWidth, 0.0f, (float)scrHeight, NEAR, FAR);
        default:
            throw std::invalid_argument("ERROR::CAMERA::WRONG_CAMERA_MODE");
        }
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processKeyboard(CameraMovement direction, float deltaTime)
    {
        switch (direction)
        {
        case (CameraMovement::FORWARD):
            this->position += deltaTime * this->movementSpeed * this->cameraFront;
            break;
        case (CameraMovement::BACKWARD):
            this->position -= deltaTime * this->movementSpeed * this->cameraFront;
            break;
        case (CameraMovement::LEFT):
            this->position -= deltaTime * this->movementSpeed
                * glm::normalize(glm::cross(this->cameraFront, this->cameraUp));
            break;
        case (CameraMovement::RIGHT):
            this->position += deltaTime * this->movementSpeed
                * glm::normalize(glm::cross(this->cameraFront, this->cameraUp));
            break;
        default:
            throw std::invalid_argument("ERROR::CAMERA::WRONG_CAMERA_DIRECTION");
        }
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = GL_TRUE)
    {
        // Offsets are based on the screen coordinates
        xoffset *= this->rotationSensitivity;
        yoffset *= this->rotationSensitivity;

        float pitch = this->rotation.x + yoffset;
        float yaw = this->rotation.y + xoffset;
        if (constrainPitch == GL_TRUE)
        {
            // Constrain pitch between -89 and 89 degree to prevent view from flipping
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }
        yaw = glm::mod(yaw, 360.0f);
        this->rotation.x = pitch;
        this->rotation.y = yaw;

        this->updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void processMouseScroll(float yoffset)
    {
        float zoom = this->zoom + yoffset * this->zoomSensitivity;
        if (zoom > MAX_ZOOM)
        {
            zoom = MAX_ZOOM;
        }
        if (zoom < MIN_ZOOM)
        {
            zoom = MIN_ZOOM;
        }
        this->zoom = zoom;
    }

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        glm::vec4 rotation = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
        glm::mat4 rotationMat = glm::mat4(1.0f);
        rotationMat = glm::rotate(rotationMat, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        rotationMat = glm::rotate(rotationMat, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        // rotationMat = glm::rotate(rotationMat, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        rotation = rotationMat * rotation;
        this->cameraFront = glm::normalize(glm::vec3(rotation.x, rotation.y, rotation.z));
    }
};
}

#endif
