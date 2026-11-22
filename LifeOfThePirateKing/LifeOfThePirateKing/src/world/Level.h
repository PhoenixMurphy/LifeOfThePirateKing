#pragma once

#include <string>
#include <vector>

#include "world/Arena.h"   // for Stage

struct LevelInfo
{
    std::string              name;
    Stage                    stage = Stage::Prairie;
    std::vector<std::string> layout;
    int                      firstWave = 1;
    float                    timeLimit = 30.0f;
    bool                     isBoss = false;
    int                      bossHealth = 0;
    int                      coinBonus = 1;
};