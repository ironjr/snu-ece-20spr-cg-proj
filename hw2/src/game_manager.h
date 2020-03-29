#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H


#include "system_global.h"
#include "camera.h"
#include "environment_manager.h"
#include "interface.h"
//#include "player_controller.h"


namespace engine
{
class GameManager
{
public:
	// TODO make this default camera reside in the player controller and let this be a pointer to it.
	engine::Camera *defaultCamera;
	engine::EnvironmentManager *envManager;
	//engine::PlayerController playerController;

	GameManager()
	{
		envManager = new engine::EnvironmentManager();
		defaultCamera = new engine::Camera(glm::vec3(0.0f, 1.0f, 2.0f));
	}
	~GameManager()
	{
		free(envManager);
		free(defaultCamera);
	}

	void processCommands(const Commands *cmd)
	{
		float deltaTime = timer.getDeltaTime();

		// TODO send these to the player controller
		if (cmd->moveForward == 1)
		{
			this->defaultCamera->processKeyboard(
				engine::CameraMovement::FORWARD,
				deltaTime,
				cmd->sprint == 1
			);
		}
		else if (cmd->moveForward == -1)
		{
			this->defaultCamera->processKeyboard(
				engine::CameraMovement::BACKWARD,
				deltaTime,
				cmd->sprint == 1
			);
		}
		if (cmd->moveRight == 1)
		{
			this->defaultCamera->processKeyboard(
				engine::CameraMovement::RIGHT,
				deltaTime,
				cmd->sprint == 1
			);
		}
		else if (cmd->moveRight == -1)
		{
			this->defaultCamera->processKeyboard(
				engine::CameraMovement::LEFT,
				deltaTime,
				cmd->sprint == 1
			);
		}
		if (cmd->moveUp == 1)
		{
			this->defaultCamera->processKeyboard(
				engine::CameraMovement::UP,
				deltaTime,
				cmd->sprint == 1
			);
		}
		else if (cmd->moveUp == -1)
		{
			this->defaultCamera->processKeyboard(
				engine::CameraMovement::DOWN,
				deltaTime,
				cmd->sprint == 1
			);
		}

		envManager->processCommands(cmd);
	}
};
}

#endif
