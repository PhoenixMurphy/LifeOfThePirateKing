#pragma once

#include "raylib.h"

struct Bullet
{
	Vector2 position{ 0.0f, 0.0f };
	Vector2 velocity{ 0.0f, 0.0f };
	float radius = 4.0f;
};