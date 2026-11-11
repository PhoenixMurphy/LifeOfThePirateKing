#pragma once

#include <cstddef>
#include <vector>

#include "raylib.h"
#include "entities/Bullet.h"

class Arena;

class BulletManager
{
public:
	BulletManger();

	void Spawn(Vector2 position, Vector2 velocity, float radius);

	void Update(float dt, const Arena& arena);
	void Draw() const;

	void Clear()
	{
		m_bullets.clear();
	}

	std::size_t Count() const
	{
		return m_bullets.size();
	}

	std::vector<Bullet>& All()
	{
		return m_bullets;
	}

	const std::vector<Bullet>& All() const
	{
		return m_bullets;
	}

	void RemoveAt(std::size_t index);
private:
	std::vector<Bullet> m_bullets;
};