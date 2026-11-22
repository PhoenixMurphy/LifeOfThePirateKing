#pragma once

#include <cstddef>

#include "containers/SinglyLinkedList.h"
#include "world/Level.h"

class LevelSequence
{
public:
    void Build();

    const LevelInfo* Current() const;

    const LevelInfo* Next() const;

    bool Advance();

    void Restart() 
    { 
        m_current = m_levels.Head(); 
    }

    int Number() const;

    std::size_t Count() const { return m_levels.Size(); }

private:
    SinglyLinkedList<LevelInfo>       m_levels;
    SinglyLinkedList<LevelInfo>::Node* m_current = nullptr;
};