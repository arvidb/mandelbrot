#pragma once

#include <stdint.h>

struct vector3
{
    constexpr vector3() noexcept
        : x(0.0f), y(0.0f), z(0.0f)
    {};
    
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
    constexpr vector2() noexcept
        : x(0.0f), y(0.0f)
    {};
    
    constexpr vector2(float x, float y) noexcept
        : x(x), y(y)
    {}
    
    float x, y;
};


