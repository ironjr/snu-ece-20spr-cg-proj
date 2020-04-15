#ifndef INTERFACE_H
#define INTERFACE_H

#include "system_global.h"


namespace engine
{
struct Commands
{
    bool firstMouse = true;

    int moveForward = 0;
    int moveRight = 0;
    int moveUp = 0;
    int sprint = 0;
    int moreDay = 0;
	float lastX = screen.width * 0.5f;
	float lastY = screen.height * 0.5f;

    bool saveCurrentTransform = false;
    bool returnToCheckpointEuler = false;
    bool returnToCheckpointQuat = false;

    void reset()
    {
        this->moveForward = 0;
        this->moveRight = 0;
		this->moveUp = 0;
        this->sprint = 0;
		this->moreDay = 0;

		this->saveCurrentTransform = false;
		this->returnToCheckpointEuler = false;
		this->returnToCheckpointQuat = false;
    }
};
}

#endif
