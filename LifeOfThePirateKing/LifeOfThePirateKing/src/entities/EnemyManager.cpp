#include "entities/EnemyManager.h"

#include <cmath>

#include "raymath.h"

#include "core/Config.h"
#include "data/Definitions.h"
#include "world/Arena.h"

static bool IsCharging(const Enemy& e)
{
    if (e.type != EnemyType::Boss)
        return false;
    const float intoCycle = std::fmod(e.ability, Config::BossChargeEvery);
    return intoCycle > Config::BossChargeEvery - Config::BossChargeTime;
}

Enemy& EnemyManager::Spawn(EnemyType type, Vector2 position, const Definitions& defs)
{
    const EnemyDef& def = defs.Enemy(type);

    Enemy e;
    e.type = type;
    e.position = position;
    e.speed = def.speed;
    e.radius = def.radius;
    e.health = def.health;
    e.maxHealth = def.health;
    e.coins = def.coins;
    e.colour = def.colour;

    m_enemies.push_back(e);
    return m_enemies.back();
}

const Enemy* EnemyManager::FindFirst(EnemyType type) const
{
    for (const Enemy& e : m_enemies)
    {
        if (e.alive && e.type == type)
            return &e;
    }
    return nullptr;
}

void EnemyManager::Update(float dt, Vector2 target, const Arena& arena)
{
    for (Enemy& e : m_enemies)
    {
        if (e.hitFlash > 0.0f)
            e.hitFlash -= dt;

        float speed = e.speed;
        if (IsCharging(e))
        {
            speed = e.speed * Config::BossChargeSpeed;
        }

        // Head straight for the target (the player).
        Vector2 toTarget = Vector2Subtract(target, e.position);
        if (Vector2Length(toTarget) < 0.5f)
            continue;
        const Vector2 dir = Vector2Normalize(toTarget);
        const float step = speed * dt;

        const Vector2 tryX{ e.position.x + dir.x * step, e.position.y };
        if (!arena.CircleHitsWall(tryX, e.radius))
            e.position.x = tryX.x;

        const Vector2 tryY{ e.position.x, e.position.y + dir.y * step };
        if (!arena.CircleHitsWall(tryY, e.radius))
            e.position.y = tryY.y;
    }
}

void EnemyManager::RemoveDead()
{
    for (auto it = m_enemies.begin(); it != m_enemies.end(); )
    {
        if (!it->alive)
            it = m_enemies.erase(it);
        else
            ++it;
    }
}

void EnemyManager::Draw() const
{
    for (const Enemy& e : m_enemies)
    {
        const Color body = (e.hitFlash > 0.0f) ? WHITE : e.colour;

        if (IsCharging(e))
        {
            DrawCircleV(e.position, e.radius + 8.0f, Fade(RED, 0.6f));
        }

        DrawCircleV(e.position, e.radius + 2.0f, BLACK);   // outline
        DrawCircleV(e.position, e.radius, body);

        // Two eyes so it's clearly a creature, not a cannonball
        const float eye = e.radius * 0.18f + 1.0f;
        DrawCircleV(Vector2{ e.position.x - e.radius * 0.35f, e.position.y - e.radius * 0.2f }, eye, BLACK);
        DrawCircleV(Vector2{ e.position.x + e.radius * 0.35f, e.position.y - e.radius * 0.2f }, eye, BLACK);

        // Health bar for enemies that take more than one hit
        if (e.maxHealth > 1 && e.type != EnemyType::Boss)
        {
            const float width = e.radius * 2.0f;
            const float left = e.position.x - e.radius;
            const float top = e.position.y - e.radius - 8.0f;
            const float frac = static_cast<float>(e.health) / static_cast<float>(e.maxHealth);
            DrawRectangleRec(Rectangle{ left, top, width, 4.0f }, BLACK);
            DrawRectangleRec(Rectangle{ left, top, width * frac, 4.0f }, RED);
        }
    }
}