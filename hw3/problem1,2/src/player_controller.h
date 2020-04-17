#ifndef PLAYER_CONTROLLER_H
#define PLAYER_CONTROLLER_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "camera.h"
#include "engine_global.h"
#include "interface.h"


namespace engine
{
class PlayerController
{
public:
	Camera* playerCam;

	PlayerController()
	{
        this->playerCam = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	}

	~PlayerController()
	{
        free(this->playerCam);
    }

	void processCommands(const Commands* cmd)
	{
		float deltaTime = timer.getDeltaTime();

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
	}
};
}

#endif
