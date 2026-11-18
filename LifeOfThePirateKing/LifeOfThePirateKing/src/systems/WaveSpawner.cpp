#include "systems/WaveSpawner.h"
#include "core/Config.h"
#include "entities/EnemyManager.h"

void WaveSpawner::SetGates(const std::vector<Gate>& gates)
{
	m_gates.Clear();
	for (const Gate& gate : gates)
	{
		m_gates.Push(gate);
	}
}

void WaveSpawner::StartWave(int waveNumber, float delay)
{
	m_wave = waveNumber;
	m_timer = delay;

	m_queue = std::queue<EnemyType>();

	const int count = 4 + 2 * waveNumber;
	for (int i = 0; i < count; ++i)
	{
		EnemyType type = EnemyType::Skeleton;
		if (i % 3 == 2)
		{
			type = EnemyType::Crab;
		}
		if (waveNumber >= 3 && i % 5 == 4)
		{
			type = EnemyType::Brute;
		}

		m_queue.push(type);
	}
}

void WaveSpawner::Update(float dt, EnemyManager& enemies, const Definitions& defs)
{
	m_timer -= dt;
	if (m_timer > 0.0f)
	{
		return;
	}

	if (m_queue.empty())
	{
		StartWave(m_wave + 1, 0.0f);
		return;
	}

	const EnemyType type = m_queue.front();
	m_queue.pop();

	if (const Gate* gate = m_gates.Current())
	{
		enemies.Spawn(type, gate->position, defs);
		m_gates.Advance();
	}

	if (m_queue.empty())
	{
		m_timer = Config::WaveBreak;
	}
	else
	{
		m_timer = Config::SpawnInterval;
	}
}