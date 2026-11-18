#pragma once

#include <unordered_map>
#include "raylib.h"

enum class EnemyType
{
	Crab,
	Skeleton,
	Brute
};

struct EnemyDef
{
	const char* name = "";
	float speed = 0.0f;
	float radius = 0.0f;
	int health = 1;
	int coins = 0;
	Color colour = WHITE;
};

class Definitions
{
public:
	Definitions();

	const EnemyDef& Enemy(EnemyType type) const;
private:
	std::unordered_map<EnemyType, EnemyDef> m_enemies;
};