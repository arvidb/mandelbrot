#pragma once

#include <stdint.h>

struct vector3
{
    constexpr vector3(float x, float y, float z) noexcept
        : x(x), y(y), z(z)
    {}
    
    union {
        struct {
            float x, y, z;
        };
        struct {
            float r, g, b;
        };
    };
};

struct vector2
{
    constexpr vector2(float x, float y) noexcept
        : x(x), y(y)
    {}
    
    struct {
        float x, y;
    };
};


