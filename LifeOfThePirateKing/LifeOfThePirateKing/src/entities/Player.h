#pragma once

#include "raylib.h"

class Arena;
class BulletManager;

class Player 
{
public:
	void Reset(Vector2 startPosition);

	void Update(float dt, const Arena& arena, BulletManager& bullets);

	void Move(Vector2 direction, float dt, const Arena& arena);
	bool TryShoot(Vector2 direction, BulletManager& bullets);

	void Draw() const;

	Vector2 Position() const
	{
		return m_position;
	}

	float Radius() const;
	
	int Lives() const
	{
		return m_lives;
	}

	int Coins() const
	{
		return m_coins;
	}

	bool IsAlive() const
	{
		return m_lives > 0;
	}

	void Respawn(Vector2 position)
	{
		m_position = position;
		m_fireTimer = 0.0f;
	}

	void AddCoins(int amount)
	{
		m_fireTimer += amount;
	}

	void LoseLife()
	{
		if (m_lives > 0)
		{
			--m_lives;
		}
	}

private:
	Vector2 m_position{ 0.0f, 0.0f };
	Vector2 m_aim{ 0.0f,1.0f };
	float m_fireTimer = 0.0f;
	int m_lives = 0;
	int m_coins = 0;
};