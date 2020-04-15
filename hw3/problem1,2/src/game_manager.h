#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H


#include "system_global.h"
#include "camera.h"
#include "interface.h"
#include "player_controller.h"


namespace engine
{
class GameManager
{
public:
	// TODO make this default camera reside in the player controller and let this be a pointer to it.
	Camera *defaultCamera;
	PlayerController *playerController;

	GameManager()
	{
		this->playerController = new PlayerController();
		this->defaultCamera = this->playerController->playerCam;
	}

	~GameManager()
	{
		free(this->playerController);
	}

	void processCommands(const Commands *cmd)
	{
		this->playerController->processCommands(cmd);
	}

    void processMouseMovement(float xoffset, float yoffset)
    {
        this->defaultCamera->processMouseMovement(xoffset, yoffset);
    }

    void processMouseScroll(float xoffset, float yoffset)
    {
        this->defaultCamera->processMouseScroll(yoffset);
    }
};
}

#endif
