#pragma once

#include <cstddef>
#include <queue>
#include <vector>

#include "containers/CircularList.h"
#include "data/Definitions.h"
#include "world/Arena.h"

class EnemyManager;

class WaveSpawner
{
public:
	void SetGates(const std::vector<Gate>& gates);

	void StartWave(int waveNumber, float delay);

	void RestartWave(float delay)
	{
		StartWave(m_wave, delay);
	}

	void Update(float dt, EnemyManager& enemies, const Definitions& defs);

	int WaveNumber() const
	{
		return m_wave;
	}

	std::size_t Remaining() const
	{
		return m_queue.size();
	}

private:
	std::queue<EnemyType> m_queue;
	CircularList<Gate> m_gates;
	int m_wave = 0;
	float m_timer = 0.0f;
};