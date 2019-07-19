#pragma once

#include "pch.h"


struct Gfx_reader {
    Gfx_reader();
    virtual ~Gfx_reader();

    bool read_archive(const std::string& filename);
};