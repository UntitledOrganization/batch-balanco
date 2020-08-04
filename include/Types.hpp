#pragma once

#include <stdint.h> 

namespace sbb
{

#pragma pack(push, 1)

	struct Rect {
		int32_t x;
		int32_t y;
		uint32_t w;
		uint32_t h;
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
