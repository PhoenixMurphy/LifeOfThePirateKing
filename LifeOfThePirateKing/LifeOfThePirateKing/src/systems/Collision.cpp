#include "systems/Collision.h"

#include <cstddef>
#include <vector>

#include "core/Config.h"
#include "entities/BulletManager.h"
#include "entities/EnemyManager.h"
#include "entities/Player.h"

namespace Collision
{

    int BulletsVsEnemies(BulletManager& bullets, EnemyManager& enemies)
    {
        int coinsEarned = 0;
        std::vector<Bullet>& all = bullets.All();

        for (std::size_t i = 0; i < all.size(); )
        {
            const Bullet& bullet = all[i];
            bool hit = false;

            for (Enemy& enemy : enemies.All())
            {
                if (!enemy.alive)
                {
                    continue;
                }

                if (CheckCollisionCircles(bullet.position, bullet.radius, enemy.position, enemy.radius))
                {
                    enemy.health -= 1;
                    enemy.hitFlash = Config::HitFlashTime;
                    if (enemy.health <= 0)
                    {
                        enemy.alive = false;
                        coinsEarned += enemy.coins;
                    }
                    hit = true;
                    break;
                }
            }

            if (hit)
            {
                bullets.RemoveAt(i);
            }
            else
            {
                ++i;
            }
        }

        return coinsEarned;
    }

    bool EnemyTouchesPlayer(const EnemyManager& enemies, const Player& player)
    {
        for (const Enemy& enemy : enemies.All())
        {
            if (enemy.alive && CheckCollisionCircles(enemy.position, enemy.radius, player.Position(), player.Radius()))
                return true;
        }
        return false;
    }

}