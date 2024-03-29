#ifndef INTERFACE_H
#define INTERFACE_H

#include "system_global.h"


namespace engine
{
constexpr float KEYBOARD_TOGGLE_DELAY = 0.3f; // seconds

struct Commands
{
    bool firstMouse = true;

    int moveForward = 0;
    int moveRight = 0;
    int moveUp = 0;
    int sprint = 0;
	float lastX = screen.width * 0.5f;
	float lastY = screen.height * 0.5f;

    bool splineOuterLineOn = false;
    bool surfaceFillOn = false;


    void reset()
    {
        this->moveForward = 0;
        this->moveRight = 0;
		this->moveUp = 0;
        this->sprint = 0;
    }

    void resetAll()
    {
        this->moveForward = 0;
        this->moveRight = 0;
		this->moveUp = 0;
        this->sprint = 0;

        this->splineOuterLineOn = false;
        this->surfaceFillOn = false;
    }
};
}

#endif
