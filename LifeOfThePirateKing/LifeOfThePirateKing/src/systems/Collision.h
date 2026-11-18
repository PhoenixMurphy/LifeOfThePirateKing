#pragma once

class BulletManager;
class EnemyManager;
class Player;

namespace Collision
{
	int BulletsVsEnemies(BulletManager& bullets, EnemyManager& enemies);

	bool EnemyTouchesPlayer(const EnemyManager& enemies, const Player& player);
}