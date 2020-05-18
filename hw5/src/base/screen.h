#ifndef SCRREN_H
#define SCREEN_H

#include <string>


namespace engine
{
// Initial and keyboard setting
//bool isKeyboardProcessed[1024] = {0};

// Defaults for screen settings.
constexpr unsigned int DEFAULT_SCR_WIDTH = 800;
constexpr unsigned int DEFAULT_SCR_HEIGHT = 600;

struct ScreenInfo
{
    // Screen settings.
    bool isWindowed = true;
    bool isKeyboardDone[1024] = { 0 };
    unsigned int width = DEFAULT_SCR_WIDTH;
    unsigned int height = DEFAULT_SCR_HEIGHT;
    std::string name = std::string("2019-20239 Jaerin Lee");
};
}

#endif
