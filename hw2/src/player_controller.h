#ifndef PLAYER_CONTROLLER_H
#define PLAYER_CONTROLLER_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "camera.h"
#include "engine_global.h"
#include "interface.h"

#include <iostream>


namespace engine
{
enum class PlayerState
{
	Moving,
	InterpEuler,
	InterpQuat
};

const float DEFAULT_PC_RETURN_TIME = 5.0f; // seconds

class PlayerController
{
public:
	Camera* playerCam;
	PlayerState playerState;

	glm::vec3 targetPosition;
	glm::vec3 targetRotation;
	float returnTime;

	PlayerController(Camera* playerCam)
		: playerState(PlayerState::Moving), playerCam(playerCam),
		returnTime(DEFAULT_PC_RETURN_TIME)
	{
		this->saveTargetTransform();
	}

	void processCommands(const Commands* cmd)
	{
		float deltaTime = timer.getDeltaTime();
		float alpha = 0.0f;
		
		switch (playerState)
		{
		case PlayerState::Moving:
			if (cmd->saveCurrentTransform == 1)
			{
				this->saveTargetTransform();
			}
			else if (cmd->returnToCheckpointEuler == 1)
			{
				std::cout << "Interpolating rotation with Euler angle notation" << std::endl;
				this->saveStartpointTransform();
				this->elapsedTime = 0.0f;
				playerState = PlayerState::InterpEuler;
				break;
			}
			else if (cmd->returnToCheckpointQuat == 1)
			{
				std::cout << "Interpolating rotation with quaternion notation" << std::endl;
				this->saveStartpointTransform();
				this->elapsedTime = 0.0f;
				playerState = PlayerState::InterpQuat;
				break;
			}

			if (cmd->moveForward == 1)
			{
				this->playerCam->processKeyboard(
					CameraMovement::FORWARD, deltaTime, cmd->sprint == 1
				);
			}
			else if (cmd->moveForward == -1)
			{
				this->playerCam->processKeyboard(
					CameraMovement::BACKWARD, deltaTime, cmd->sprint == 1
				);
			}
			if (cmd->moveRight == 1)
			{
				this->playerCam->processKeyboard(
					CameraMovement::RIGHT, deltaTime, cmd->sprint == 1
				);
			}
			else if (cmd->moveRight == -1)
			{
				this->playerCam->processKeyboard(
					CameraMovement::LEFT, deltaTime, cmd->sprint == 1
				);
			}
			if (cmd->moveUp == 1)
			{
				this->playerCam->processKeyboard(
					CameraMovement::UP, deltaTime, cmd->sprint == 1
				);
			}
			else if (cmd->moveUp == -1)
			{
				this->playerCam->processKeyboard(
					CameraMovement::DOWN, deltaTime, cmd->sprint == 1
				);
			}
			break;
		case PlayerState::InterpEuler:
			this->elapsedTime += deltaTime;
			alpha = this->elapsedTime / this->returnTime;
			// Target is close enough
			if (alpha > 0.999f)
			{
				alpha = 1.0f;
				playerState = PlayerState::Moving;
			}
			this->updateTransformWithEuler(alpha);
			break;
		case PlayerState::InterpQuat:
			this->elapsedTime += deltaTime;
			alpha = this->elapsedTime / this->returnTime;
			// Target is close enough
			if (alpha > 0.999f)
			{
				alpha = 1.0f;
				playerState = PlayerState::Moving;

				// Last update should be done with Euler so we can reset the
				// roll to zero.
				this->updateTransformWithEuler(alpha);
			}
			else
			{
				this->updateTransformWithQuat(alpha);
			}
			break;
        default:
            throw std::invalid_argument("ERROR::PLAYER_CONTROLLER::WRONG_STATE");
		}
	}

private:
	glm::vec3 startpointPosition;
	glm::vec3 startpointRotation;
	float elapsedTime;

	void saveTargetTransform()
	{
		this->targetPosition = playerCam->position;
		this->targetRotation = playerCam->rotation;
	}

	void saveStartpointTransform()
	{
		this->startpointPosition = playerCam->position;
		this->startpointRotation = playerCam->rotation;
	}

	void updateTransformWithEuler(float alpha)
	{

		// Update to the new rotation.
		// Pitch \in [  -89.0f,  89.0f ]
		// Yaw   \in [ -180.0f, 180.0f ]
		float newPitch = this->targetRotation.x * alpha
			+ this->startpointRotation.x * (1 - alpha);
		float targetYaw = this->targetRotation.y;
		float startpointYaw = this->startpointRotation.y;
		float diffYaw = targetYaw - startpointYaw;
		float newYaw
			= (diffYaw > 180.0f)
			? (targetYaw * alpha + (startpointYaw + 360.0f) * (1 - alpha))
			: (diffYaw < -180.0f)
			? ((targetYaw + 360.0f) * alpha + startpointYaw * (1 - alpha))
			: (targetYaw * alpha + startpointYaw * (1 - alpha));
		newYaw = fmod(newYaw, 360.0f);
		float newRoll = 0.0f;
		glm::vec3 newRotation = glm::vec3(newPitch, newYaw, newRoll);
		this->playerCam->updateOrientation(newRotation);

		this->playerCam->position
			= this->targetPosition * alpha + this->startpointPosition * (1 - alpha);
	}

	void updateTransformWithQuat(float alpha)
	{
		glm::quat target = this->eulerToQuat(this->targetRotation);
		glm::quat startpoint = this->eulerToQuat(this->startpointRotation);
		glm::quat interp = glm::mix(startpoint, target, alpha);
		this->playerCam->updateOrientation(interp);

		this->playerCam->position
			= targetPosition * alpha + startpointPosition * (1 - alpha);
	}

	glm::quat eulerToQuat(glm::vec3 eulerAngle)
	{
		// Pitch
		glm::quat rotZ = glm::angleAxis(
			glm::radians(eulerAngle.x),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);
		// Yaw
		glm::quat rotY = glm::angleAxis(
			glm::radians(eulerAngle.y),
			glm::vec3(0.0f, -1.0f, 0.0f)
		);
		// Roll
		glm::quat rotX = glm::angleAxis(
			glm::radians(eulerAngle.z),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		// Roll -> Pitch -> Yaw
		return rotY * rotZ * rotX;
	}
};
}

#endif