#pragma once

#include <cstddef>
#include <list>

#include "raylib.h"
#include "entities/Enemy.h"

class Arena;
class Definitions;

class EnemyManager
{
public:
	void Spawn(EnemyType type, Vector2 position, const Definitions& defs);

	void Update(float dt, Vector2 target, const Arena& arena);

	void RemoveDead();

	void Draw() const;
	
	void Clear()
	{
		m_enemies.clear();
	}

	std::size_t Count() const
	{
		return m_enemies.size();
	}

	std::list<Enemy>& All()
	{
		return m_enemies;
	}

	const std::list<Enemy>& All() const
	{
		return m_enemies;
	}

private:
	std::list<Enemy> m_enemies;
};