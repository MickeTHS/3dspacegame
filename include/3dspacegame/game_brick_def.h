#pragma once

#include "pch.h"

struct Game_brick_coord {
    int32_t x;
    int32_t y;
    int32_t z;
};

struct Game_brick_instance {
    Game_brick_coord coord;
    uint32_t sampler_index;
};