#pragma once

#include "raylib.h"
#include "data/Definitions.h"

struct Enemy
{
	EnemyType type = EnemyType::Skeleton;
	Vector2 position{ 0.0f, 0.0f };
	float speed = 0.0f;
	float radius = 0.0f;
	int health = 1;
	int maxHealth = 1;
	int coins = 1;
	Color colour = WHITE;

	float hitFlash = 0.0f;
	float ability = 0.0f;
	bool alive = true;
};