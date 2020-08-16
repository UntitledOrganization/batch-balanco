#pragma once

#include <stdint.h> 

namespace sbb
{

#pragma pack(push, 1)

    struct Rect {
        float x;
        float y;
        float w;
        float h;
    };

    /*
     * Vectors
     */

    struct Vec2 {
        union {
            float x;
            float r;
        };
        union {
            float y;
            float g;
        };
    };

    struct Vec3 {
        union {
            float x;
            float r;
        };
        union {
            float y;
            float g;
        };
        union {
            float z;
            float b;
        };
    };

    struct Vec4 {
        union {
            float x;
            float r;
        };
        union {
            float y;
            float g;
        };
        union {
            float z;
            float b;
        };
        union {
            float w;
            float a;
        };
    };

#pragma pack(pop)
}
