#pragma once

#include "pch.h"

struct Game_brick_coord {
    int16_t x;
    int16_t y;
    int16_t z;
};

struct Game_brick_instance {
    Game_brick_coord coord;
    uint32_t sampler_index;
};