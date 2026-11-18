#include "data/Definitions.h"

#include "core/Config.h"

Definitions::Definitions()
{
    // Sizes and speeds are multiples of TileSize, like the player's,
    // so the game plays the same at any tile size.
    const float tile = static_cast<float>(Config::TileSize);

    EnemyDef crab;
    crab.name = "Crab";
    crab.speed = tile * 3.5f;
    crab.radius = tile * 0.28f;
    crab.health = 1;
    crab.coins = 1;
    crab.colour = Color{ 220, 80, 60, 255 };
    m_enemies[EnemyType::Crab] = crab;

    EnemyDef skeleton;
    skeleton.name = "Skeleton";
    skeleton.speed = tile * 2.5f;
    skeleton.radius = tile * 0.32f;
    skeleton.health = 1;
    skeleton.coins = 1;
    skeleton.colour = Color{ 235, 235, 220, 255 };
    m_enemies[EnemyType::Skeleton] = skeleton;

    EnemyDef brute;
    brute.name = "Brute";
    brute.speed = tile * 1.6f;
    brute.radius = tile * 0.42f;
    brute.health = 3;
    brute.coins = 3;
    brute.colour = Color{ 90, 120, 70, 255 };
    m_enemies[EnemyType::Brute] = brute;
}

const EnemyDef& Definitions::Enemy(EnemyType type) const
{
    return m_enemies.at(type);
}