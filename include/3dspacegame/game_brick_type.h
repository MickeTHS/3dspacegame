#pragma once

#include "pch.h"

/** should probably have materials grouped somehow */
enum Game_brick_basetype {
    GBT_NONE = -1,
    GBT_ASTEROID_GRAY = 0
};

struct Game_brick_type {

};

struct Game_brick_data {
    uint32_t material; // shader options
    uint32_t texture; // which texture id
};