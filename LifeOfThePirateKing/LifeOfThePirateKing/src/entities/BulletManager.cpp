#include "entities/BulletManager.h"
#include "world/Arena.h"

BulletManager::BulletManger()
{
	m_bullets.reserve(256);
}

void BulletManager::Spawn(Vector2 position, Vector2 velocity, float radius)
{
	Bullet bullet;
	bullet.position = position;
	bullet.velocity = velocity;
	bullet.radius = radius;
	m_bullets.push_back(bullet);
}

void BulletManager::RemoveAt(std::size_t index)
{
	m_bullets[index] = m_bullets.back();
	m_bullets.pop_back();
}

void BulletManager::Update(float dt, const Arena& arena)
{
	for (std::size_t i = 0; i < m_bullets.size();)
	{
		Bullet& bullet = m_bullets[i];
		bullet.position += bullet.velocity.x * dt;
		bullet.position += bullet.velocity.y * dt;

		if (arena.CircleHitsSolid(bullet.position, bullet.radius))
		{
			RemoveAt(i);
		}
		else
		{
			++i;
		}
	}
}

void BulletManager::Draw() const
{
	for (const Bullet& bullet : m_bullets)
	{
		DrawCircleV(bullet.position, bullet.radius + 1.0f, BLACK);
		DrawCircleV(bullet.position, bullet.radius, GOLD);
	}
}
