#include "entities/Player.h"

#include "raymath.h"

#include "core/Config.h"
#include "entities/BulletManager.h"
#include "world/Arena.h"

void Player::Reset(Vector2 startPosition)
{
	m_position = startPosition;
	m_aim = Vector2{ 0.0f, 1.0f };
	m_fireTimer = 0.0f;
	m_lives = Config::StartingLives;
	m_coins = 0;
}

float Player::Radius() const
{
	return Config::PlayerRadius;
}

void Player::Update(float dt, const Arena& arena, BulletManager& bullets)
{
	Vector2 move{ 0.0f, 0.0f };
	if (IsKeyDown(KEY_W))
	{
		move.y -= 1.0f;
	}
	if (IsKeyDown(KEY_S))
	{
		move.y += 1.0f;
	}
	if (IsKeyDown(KEY_A))
	{
		move.x -= 1.0f;
	}
	if (IsKeyDown(KEY_D))
	{
		move.x += 1.0f;
	}

	Vector2 shoot{ 0.0f, 0.0f };
	if (IsKeyDown(KEY_UP))
	{
		shoot.y -= 1.0f;
	}
	if (IsKeyDown(KEY_DOWN))
	{
		shoot.y += 1.0f;
	}
	if (IsKeyDown(KEY_LEFT))
	{
		shoot.x -= 1.0f;
	}
	if (IsKeyDown(KEY_RIGHT))
	{
		shoot.x += 1.0f;
	}

	// Mouse: hold the left button to shoot towards the cursor.
	// The direction is from the player to the mouse; TryShoot normalises it.
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		const Vector2 mouse = GetMousePosition();
		shoot = Vector2{ mouse.x - m_position.x, mouse.y - m_position.y };
	}

	// A fresh press of an arrow key or the mouse button always fires straight
	// away, so every tap shoots. Holding the button still fires at the
	// FireCooldown rate.
	const bool newPress = IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN) ||
		IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) ||
		IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

	Move(move, dt, arena);

	m_fireTimer -= dt;
	if (newPress)
	{
		m_fireTimer = 0.0f;   // skip the rest of the cooldown for this shot
	}
	TryShoot(shoot, bullets);
}

void Player::Move(Vector2 direction, float dt, const Arena& arena)
{
	if (direction.x == 0.0f && direction.y == 0.0f)
	{
		return;
	}

	// Makes it so moving diagonals isn't faster
	direction = Vector2Normalize(direction);
	const float step = Config::PlayerSpeed * dt;

	Vector2 tryX{ m_position.x + direction.x * step, m_position.y };
	if (!arena.CircleHitsSolid(tryX, Config::PlayerRadius))
	{
		m_position.x = tryX.x;
	}

	Vector2 tryY{ m_position.x, m_position.y + direction.y * step };
	if (!arena.CircleHitsSolid(tryY, Config::PlayerRadius))
	{
		m_position.y = tryY.y;
	}
}

bool Player::TryShoot(Vector2 direction, BulletManager& bullets)
{
	if (direction.x == 0.0f && direction.y == 0.0f)
	{
		return false;
	}

	direction = Vector2Normalize(direction);
	m_aim = direction;

	if (m_fireTimer > 0.0f)
	{
		return false;
	}

	// Spawns the bullets just outside of the player
	const Vector2 muzzle = Vector2Add(m_position, Vector2Scale(direction, Config::PlayerRadius + 2.0f));
	const Vector2 velocity = Vector2Scale(direction, Config::BulletSpeed);
	bullets.Spawn(muzzle, velocity, Config::BulletRadius);

	m_fireTimer = Config::FireCooldown;
	return true;
}

void Player::Draw() const
{
	const Vector2 gunEnd = Vector2Add(m_position, Vector2Scale(m_aim, Config::PlayerRadius + 14.0f));
	DrawLineEx(m_position, gunEnd, 6.0f, DARKGRAY);

	DrawCircleV(m_position, Config::PlayerRadius + 2.0f, BLACK);
	DrawCircleV(m_position, Config::PlayerRadius, Color{ 245, 205, 160, 255 });
	DrawCircleSector(m_position, Config::PlayerRadius, 180.0f, 360.0f, 16, MAROON);
}