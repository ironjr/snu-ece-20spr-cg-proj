#ifndef ENGINE_GLOBAL_H
#define ENGINE_GLOBAL_H

#include "game_manager.h"
#include "interface.h"


// Only one instance per class
// TODO make these thread safe
engine::Commands cmd;
engine::GameManager gameManager;

#endif