#pragma once

#include "raylib.h"

struct Button
{
    Rectangle   bounds{ 0.0f, 0.0f, 0.0f, 0.0f };
    const char* label = "";
};

bool IsHovered(const Button& button);

bool IsClicked(const Button& button);

void DrawButton(const Button& button, bool highlighted);

// The title screen
class MainMenu
{
public:
    enum class Choice { None, Start, HowToPlay, Quit };

    MainMenu();

    Choice Update();
    void   Draw() const;

private:
    static constexpr int ButtonCount = 3;

    Button m_buttons[ButtonCount];
    int    m_selected = 0;
};

// The "How to play" page with a Back button
class HowToPlayScreen
{
public:
    HowToPlayScreen();

    bool Update();
    void Draw() const;

private:
    Button m_back;
};