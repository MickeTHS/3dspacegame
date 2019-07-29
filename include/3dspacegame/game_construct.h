#pragma once

#include "pch.h"
#include "3dspacegame/game_brick_def.h"

struct Game_contruct_brick_group {
    uint32_t material;
    std::vector<Game_brick_instance> bricks;
};

/**
 * A construct is a collection of bricks that has been created by the player
 * 
 */
struct Game_construct {

};