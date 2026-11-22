#include "world/LevelSequence.h"

#include <string>

#include "world/Layouts.h"

static const std::vector<std::string>& LayoutFor(Stage stage)
{
    switch (stage)
    {
    case Stage::Forest:    return Layouts::Forest;
    case Stage::Graveyard: return Layouts::Graveyard;
    case Stage::Prairie:
    default:               return Layouts::Prairie;
    }
}

void LevelSequence::Build()
{
    m_levels.Clear();

    const Stage stages[3] = { Stage::Prairie, Stage::Forest, Stage::Graveyard };
    SinglyLinkedList<LevelInfo>::Node* firstBoss = nullptr;

    for (int s = 0; s < 3; ++s)
    {
        for (int l = 1; l <= 3; ++l)
        {
            LevelInfo level;
            level.name = std::to_string(s + 1) + "-" + std::to_string(l);
            level.stage = stages[s];
            level.layout = LayoutFor(stages[s]);
            level.firstWave = s * 3 + l;
            level.timeLimit = 25.0f + 5.0f * static_cast<float>(s + l);
            m_levels.PushBack(level);
        }

        LevelInfo boss;
        boss.name = std::to_string(s + 1) + "-Boss";
        boss.stage = stages[s];
        boss.layout = Layouts::BossArena;
        boss.firstWave = s * 3 + 1;
        boss.isBoss = true;
        boss.bossHealth = 20 + 10 * s;

        SinglyLinkedList<LevelInfo>::Node* node = m_levels.PushBack(boss);
        if (s == 0)
            firstBoss = node; 
    }

    LevelInfo bonus;
    bonus.name = "Treasure Cove";
    bonus.stage = Stage::Prairie;
    bonus.layout = Layouts::TreasureCove;
    bonus.firstWave = 2;
    bonus.timeLimit = 20.0f;
    bonus.coinBonus = 3;
    m_levels.InsertAfter(firstBoss, bonus);

    m_current = m_levels.Head();
}

const LevelInfo* LevelSequence::Current() const
{
    if (m_current != nullptr)
    {
        return &m_current->data;
    }
    return nullptr;
}

const LevelInfo* LevelSequence::Next() const
{
    if (m_current == nullptr || m_current->next == nullptr)
        return nullptr;
    return &m_current->next->data;
}

bool LevelSequence::Advance()
{
    if (m_current == nullptr)
        return false;

    m_current = m_current->next;
    return m_current != nullptr;
}

int LevelSequence::Number() const
{
    int n = 1;
    for (auto* node = m_levels.Head(); node != nullptr && node != m_current; node = node->next)
        ++n;
    return n;
}