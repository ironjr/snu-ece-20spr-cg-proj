#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H


#include "system_global.h"
#include "camera.h"
#include "environment_manager.h"
#include "interface.h"
#include "player_controller.h"


namespace engine
{
class GameManager
{
public:
	// TODO make this default camera reside in the player controller and let this be a pointer to it.
	Camera *defaultCamera;
	EnvironmentManager *envManager;
	PlayerController *playerController;

	GameManager()
	{
		envManager = new EnvironmentManager();
		defaultCamera = new Camera(glm::vec3(0.0f, 1.0f, 2.0f));
		playerController = new PlayerController(defaultCamera);
	}
	~GameManager()
	{
		free(envManager);
		free(defaultCamera);
		free(playerController);
	}

	void processCommands(const Commands *cmd)
	{
		playerController->processCommands(cmd);
		envManager->processCommands(cmd);
	}
};
}

#endif
