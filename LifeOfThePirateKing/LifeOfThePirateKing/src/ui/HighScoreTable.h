#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <string>

class HighScoreTable
{
public:
    using Table = std::multimap<int, std::string, std::greater<int>>;

    static constexpr std::size_t MaxEntries = 10;

    bool Qualifies(int score) const;

    // Adds a score and keeps only the top 10
    void Add(int score, const std::string& name);

    bool Load(const std::string& path);
    bool Save(const std::string& path) const;

    void Draw() const;

    const Table& All() const { return m_scores; }

private:
    Table m_scores;   // highest first
};