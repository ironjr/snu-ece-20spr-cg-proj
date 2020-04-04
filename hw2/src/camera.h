#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <vector>


namespace engine
{
// Defines several possible options for camera movement. Used as abstraction
// to stay away from window-system specific input methods.
enum class CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

enum class CameraMode
{
    PERSPECTIVE,
    ORTHOGONAL
};

// Default camera values
const float DEFAULT_CAMERA_SPEED               =   2.5f;
const float DEFAULT_CAMERA_SPRINT_MULTIPLIER   =   3.0f;

const float DEFAULT_CAMERA_YAW                 = -90.0f;
const float DEFAULT_CAMERA_PITCH               =   0.0f;
const float DEFAULT_CAMERA_ROLL                =   0.0f;
const float DEFAULT_CAMERA_PITCH_SENSITIVITY   =   0.07f;
const float DEFAULT_CAMERA_YAW_SENSITIVITY     =   0.1f;
const float DEFAULT_CAMERA_ROLL_SENSITIVITY    =   0.1f;

const float DEFAULT_CAMERA_NEAR                =   0.1f;
const float DEFAULT_CAMERA_FAR                 = 100.0f;

const float DEFAULT_CAMERA_ZOOM                =  45.0f; // (FOV)
const float DEFAULT_CAMERA_MAX_ZOOM            = 170.0f;
const float DEFAULT_CAMERA_MIN_ZOOM            =  10.0f;
const float DEFAULT_CAMERA_ZOOM_SENSITIVITY    =   1.0f;


// An abstract camera class that processes input and calculates the
// corresponding Euler angles, vectors and matrices for use in OpenGL.
class Camera
{
public:
    // Camera attributes
    glm::vec3 position;
    glm::vec3 rotation; // Euler angles (pitch, yaw, roll)
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    glm::vec3 cameraRight;
    glm::vec3 worldUp;

    // Camera options
    float movementSpeed;
    float sprintMultiplier;
    glm::vec3 rotationSensitivity;
    float zoomSensitivity;
    float zoom;
    CameraMode mode;

    // Constructor with vectors.
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = DEFAULT_CAMERA_YAW,
        float pitch = DEFAULT_CAMERA_PITCH,
        float roll = DEFAULT_CAMERA_ROLL,
        CameraMode mode = CameraMode::PERSPECTIVE
    ) : cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
        movementSpeed(DEFAULT_CAMERA_SPEED), rotationSensitivity(glm::vec3(
            DEFAULT_CAMERA_PITCH_SENSITIVITY,
            DEFAULT_CAMERA_YAW_SENSITIVITY,
            DEFAULT_CAMERA_ROLL_SENSITIVITY
        )), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)), zoom(DEFAULT_CAMERA_ZOOM),
        zoomSensitivity(DEFAULT_CAMERA_ZOOM_SENSITIVITY),
        sprintMultiplier(DEFAULT_CAMERA_SPRINT_MULTIPLIER)
    {
        this->position = position;
        this->cameraUp = glm::normalize(up);
        this->cameraRight
            = glm::normalize(glm::cross(this->cameraFront, this->cameraUp));
        this->rotation = glm::vec3(pitch, yaw, roll);
        this->mode = mode;
    }

    // Constructor with scalar values.
    Camera(
        float posX, float posY, float posZ, float upX, float upY, float upZ,
        float yaw, float pitch, float roll = DEFAULT_CAMERA_ROLL,
        CameraMode mode = CameraMode::PERSPECTIVE
    ) : cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
        movementSpeed(DEFAULT_CAMERA_SPEED), rotationSensitivity(glm::vec3(
            DEFAULT_CAMERA_PITCH_SENSITIVITY,
            DEFAULT_CAMERA_YAW_SENSITIVITY,
            DEFAULT_CAMERA_ROLL_SENSITIVITY
        )), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)), zoom(DEFAULT_CAMERA_ZOOM),
        zoomSensitivity(DEFAULT_CAMERA_ZOOM_SENSITIVITY),
        sprintMultiplier(DEFAULT_CAMERA_SPRINT_MULTIPLIER)
    {
        this->position = glm::vec3(posX, posY, posZ);
        this->cameraUp = glm::normalize(glm::vec3(upX, upY, upZ));
        this->cameraRight
            = glm::normalize(glm::cross(this->cameraFront, this->cameraUp));
        this->rotation = glm::vec3(pitch, yaw, roll);
        this->mode = mode;
    }

    // Returns the view matrix calculated using Euler angles and the LookAt matrix.
    glm::mat4 getViewMatrix()
    {
        return glm::lookAt(this->position, this->position + this->cameraFront, this->cameraUp);
    }

    // Returns the projection matrix calculated using field of view and screen size.
    glm::mat4 getProjectionMatrix(unsigned int scrWidth, unsigned int scrHeight)
    {
        switch (this->mode)
        {
        case CameraMode::PERSPECTIVE:
            return glm::perspective(
                glm::radians(this->zoom),
                (float)scrWidth / scrHeight,
                DEFAULT_CAMERA_NEAR, DEFAULT_CAMERA_FAR
            );
        case CameraMode::ORTHOGONAL:
            return glm::ortho(
                0.0f, (float)scrWidth,
                0.0f, (float)scrHeight,
                DEFAULT_CAMERA_NEAR, DEFAULT_CAMERA_FAR
            );
        default:
            throw std::invalid_argument("ERROR::CAMERA::WRONG_CAMERA_MODE");
        }
    }

    // TODO : This should be in the player controller.
    // Processes input received from any keyboard-like input system. Accepts
    // input parameter in the form of camera defined ENUM (to abstract it from
    // windowing systems).
    void processKeyboard(CameraMovement direction, float deltaTime, bool sprint=false)
    {
        float speed = this->movementSpeed;
        if (sprint)
        {
            speed *= this->sprintMultiplier;
        }
        switch (direction)
        {
        case CameraMovement::FORWARD:
            this->position += deltaTime * speed * this->cameraFront;
            break;
        case CameraMovement::BACKWARD:
            this->position -= deltaTime * speed * this->cameraFront;
            break;
        case CameraMovement::LEFT:
            this->position -= deltaTime * speed
                * glm::normalize(glm::cross(this->cameraFront, this->cameraUp));
            break;
        case CameraMovement::RIGHT:
            this->position += deltaTime * speed
                * glm::normalize(glm::cross(this->cameraFront, this->cameraUp));
            break;
        case CameraMovement::UP:
            this->position += deltaTime * speed * this->cameraUp;
            break;
        case CameraMovement::DOWN:
            this->position -= deltaTime * speed * this->cameraUp;
            break;
        default:
            throw std::invalid_argument("ERROR::CAMERA::WRONG_CAMERA_DIRECTION");
        }
    }

    // Processes input received from a mouse input system. Expects the offset
    // value in both the x and y direction.
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = GL_TRUE)
    {
        // Offsets are based on the screen coordinates
        xoffset *= this->rotationSensitivity.y;
        yoffset *= this->rotationSensitivity.x;

        float pitch = this->rotation.x + yoffset;
        float yaw = this->rotation.y + xoffset;
        if (constrainPitch == GL_TRUE)
        {
            // Constrain pitch between -89 and 89 degree to prevent view from
            // flipping.
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }
        yaw = glm::mod(glm::mod(yaw, 360.0f) + 360.0f, 360.0f);
        if (yaw > 180.0f)
            yaw -= 360.0f;
        this->rotation.x = pitch;
        this->rotation.y = yaw;

        this->updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires
    // input on the vertical wheel-axis.
    void processMouseScroll(float yoffset)
    {
        float zoom = this->zoom + yoffset * this->zoomSensitivity;
        if (zoom > DEFAULT_CAMERA_MAX_ZOOM)
        {
            zoom = DEFAULT_CAMERA_MAX_ZOOM;
        }
        if (zoom < DEFAULT_CAMERA_MIN_ZOOM)
        {
            zoom = DEFAULT_CAMERA_MIN_ZOOM;
        }
        this->zoom = zoom;
    }

    void updateOrientation(glm::vec3 eulerAngle)
    {
        this->rotation = eulerAngle;
        this->updateCameraVectors();
        
        // Log the camera right vector
        std::cout << std::setw(7) << std::setprecision(3)
            << this->cameraRight.x << "  "
            << this->cameraRight.y << "  "
            << this->cameraRight.z << std::endl;
    }

    void updateOrientation(glm::quat quaternion)
	{
		this->cameraFront = quaternion * glm::vec3(1.0f, 0.0f, 0.0f);
        this->cameraRight = quaternion * glm::vec3(0.0f, 0.0f, 1.0f);
        this->cameraUp = quaternion * glm::vec3(0.0f, 1.0f, 0.0f);

        // Log the camera right vector
		std::cout << std::setw(7) << std::setprecision(3)
			<< this->cameraRight.x << "  "
			<< this->cameraRight.y << "  "
			<< this->cameraRight.z << std::endl;
	}

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles.
    void updateCameraVectors()
    {
        float pitch = this->rotation.x;
        float yaw = this->rotation.y;
        float roll = this->rotation.z;

        float cosp = cos(glm::radians(pitch));
        float sinp = sin(glm::radians(pitch));
        float cosy = cos(glm::radians(yaw));
        float siny = sin(glm::radians(yaw));
        float cosr = cos(glm::radians(roll));
        float sinr = sin(glm::radians(roll));

        // Rotations are applied in the order of yaw -> pitch -> roll.
        this->cameraFront = glm::normalize(glm::vec3(
            cosy * cosp * cosr - sinp * sinr,
            cosy * cosp * sinr + sinp * cosr,
            siny * cosp
        ));
        this->cameraRight
            = glm::normalize(glm::cross(this->cameraFront, this->worldUp));
        this->cameraUp
            = glm::cross(this->cameraRight, this->cameraFront);
    }
};
}

#endif
