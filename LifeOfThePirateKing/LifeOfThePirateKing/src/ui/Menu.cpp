#include "ui/Menu.h"

#include "core/Config.h"

bool IsHovered(const Button& button)
{
    return CheckCollisionPointRec(GetMousePosition(), button.bounds);
}

bool IsClicked(const Button& button)
{
    return IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && IsHovered(button);
}

void DrawButton(const Button& button, bool highlighted)
{
    Color fill = Color{ 60, 44, 32, 255 };
    Color border = Color{ 120, 90, 60, 255 };
    Color text = RAYWHITE;
    if (highlighted)
    {
        fill = Color{ 196, 146, 40, 255 };
        border = GOLD;
        text = Color{ 30, 22, 18, 255 };
    }

    DrawRectangleRounded(button.bounds, 0.3f, 8, fill);
    DrawRectangleRoundedLinesEx(button.bounds, 0.3f, 8, 2.0f, border);

    const int fontSize = 24;
    const int textW = MeasureText(button.label, fontSize);
    DrawText(button.label,
        static_cast<int>(button.bounds.x + (button.bounds.width - static_cast<float>(textW)) / 2.0f),
        static_cast<int>(button.bounds.y + (button.bounds.height - static_cast<float>(fontSize)) / 2.0f),
        fontSize, text);
}

static void DrawCentred(const char* text, int y, int fontSize, Color colour)
{
    DrawText(text, (Config::ScreenWidth - MeasureText(text, fontSize)) / 2, y, fontSize, colour);
}

static void DrawBackdrop()
{
    DrawRectangle(0, 0, Config::ScreenWidth, Config::ScreenHeight, Fade(BLACK, 0.75f));
}

MainMenu::MainMenu()
{
    const float width = 240.0f;
    const float height = 52.0f;
    const float gap = 20.0f;
    const float left = (static_cast<float>(Config::ScreenWidth) - width) / 2.0f;
    const float top = static_cast<float>(Config::ScreenHeight) * 0.45f;

    m_buttons[0] = Button{ Rectangle{ left, top,                         width, height }, "Start" };
    m_buttons[1] = Button{ Rectangle{ left, top + (height + gap),        width, height }, "How to play" };
    m_buttons[2] = Button{ Rectangle{ left, top + (height + gap) * 2.0f, width, height }, "Quit" };
}

MainMenu::Choice MainMenu::Update()
{
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        m_selected = (m_selected + 1) % ButtonCount;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        m_selected = (m_selected + ButtonCount - 1) % ButtonCount;

    int clicked = -1;
    bool overAButton = false;
    for (int i = 0; i < ButtonCount; ++i)
    {
        if (IsHovered(m_buttons[i]))
        {
            m_selected = i;
            overAButton = true;
        }
        if (IsClicked(m_buttons[i]))
            clicked = i;
    }
    if (overAButton)
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    else
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    // Enter picks the highlighted button
    if (IsKeyPressed(KEY_ENTER))
        clicked = m_selected;

    switch (clicked)
    {
    case 0:  return Choice::Start;
    case 1:  return Choice::HowToPlay;
    case 2:  return Choice::Quit;
    default: return Choice::None;
    }
}

void MainMenu::Draw() const
{
    DrawBackdrop();

    const int titleY = static_cast<int>(static_cast<float>(Config::ScreenHeight) * 0.16f);
    DrawCentred("LIFE OF THE", titleY, 30, RAYWHITE);
    DrawCentred("PIRATE KING", titleY + 36, 56, GOLD);

    for (int i = 0; i < ButtonCount; ++i)
        DrawButton(m_buttons[i], i == m_selected);

    DrawCentred("Click a button, or use Up/Down and Enter", Config::ScreenHeight - 36, 16, GRAY);
}

HowToPlayScreen::HowToPlayScreen()
{
    const float width = 200.0f;
    const float height = 48.0f;
    m_back = Button{ Rectangle{ (static_cast<float>(Config::ScreenWidth) - width) / 2.0f,
                                static_cast<float>(Config::ScreenHeight) - height - 32.0f,
                                width, height },
                     "Back" };
}

bool HowToPlayScreen::Update()
{
    if (IsHovered(m_back))
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    else
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    return IsClicked(m_back) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_BACKSPACE);
}

void HowToPlayScreen::Draw() const
{
    DrawBackdrop();

    const int left = 32;
    int y = 32;

    DrawText("HOW TO PLAY", left, y, 36, GOLD);
    y += 56;

    DrawText("Controls", left, y, 22, RAYWHITE);
    y += 30;
    DrawText("Move:   W A S D", left, y, 18, LIGHTGRAY);                       y += 24;
    DrawText("Shoot:  arrow keys (two at once = diagonal)", left, y, 18, LIGHTGRAY); y += 24;
    DrawText("        or hold the left mouse button", left, y, 18, LIGHTGRAY); y += 24;
    DrawText("Shop:   Up/Down, Enter to buy, Space to leave", left, y, 18, LIGHTGRAY); y += 40;

    DrawText("Rules", left, y, 22, RAYWHITE);
    y += 30;
    DrawText("- Survive the timer, then clear the arena", left, y, 18, LIGHTGRAY);   y += 24;
    DrawText("- Touching an enemy costs a life", left, y, 18, LIGHTGRAY);            y += 24;
    DrawText("- Kills earn coins for the shop after a boss", left, y, 18, LIGHTGRAY);  y += 24;
    DrawText("- Boss levels end when the boss dies", left, y, 18, LIGHTGRAY);        y += 24;
    DrawText("- A red ring means the boss is charging!", left, y, 18, LIGHTGRAY);    y += 24;
    DrawText("- Beat all three stages to be The Pirate King", left, y, 18, LIGHTGRAY);   y += 40;

    DrawText("F1: debug info    F2: skip a level", left, y, 16, GRAY);

    DrawButton(m_back, IsHovered(m_back));
}