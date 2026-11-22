#include "ui/HighScoreTable.h"

#include <fstream>
#include <iterator>

#include "raylib.h"

#include "core/Config.h"

bool HighScoreTable::Qualifies(int score) const
{
    if (score <= 0)
        return false;
    if (m_scores.size() < MaxEntries)
        return true;
    return score > std::prev(m_scores.end())->first;
}

void HighScoreTable::Add(int score, const std::string& name)
{
    m_scores.insert({ score, name });

    while (m_scores.size() > MaxEntries)
        m_scores.erase(std::prev(m_scores.end()));
}

bool HighScoreTable::Load(const std::string& path)
{
    std::ifstream file(path);
    if (!file)
        return false;

    m_scores.clear();

    int score = 0;
    std::string name;
   
    while (file >> score && std::getline(file, name))
    {
        if (!name.empty() && name[0] == ' ')
            name.erase(0, 1);
        Add(score, name);
    }
    return true;
}

bool HighScoreTable::Save(const std::string& path) const
{
    std::ofstream file(path);
    if (!file)
        return false;

    for (const auto& [score, name] : m_scores)    // highest first
        file << score << ' ' << name << '\n';
    return true;
}

void HighScoreTable::Draw() const
{
    const int left = 48;
    const int top = Config::HUDHeight + 24;

    DrawRectangle(0, Config::HUDHeight, Config::ScreenWidth, Config::ScreenHeight - Config::HUDHeight,
        Fade(BLACK, 0.85f));
    DrawText("HIGH SCORES", left, top, 40, GOLD);

    int y = top + 64;
    int rank = 1;
    for (const auto& [score, name] : m_scores)
    {
        DrawText(TextFormat("%2d.", rank), left, y, 20, LIGHTGRAY);
        DrawText(name.c_str(), left + 48, y, 20, RAYWHITE);
        const char* scoreText = TextFormat("%d", score);
        DrawText(scoreText, Config::ScreenWidth - left - MeasureText(scoreText, 20), y, 20, GOLD);
        y += 30;
        ++rank;
    }

    if (m_scores.empty())
        DrawText("No scores yet - be the first!", left, y, 20, GRAY);

    DrawText("Press Enter to play again", left, Config::ScreenHeight - 40, 20, LIGHTGRAY);
}