#include "ui/Shop.h"

#include "raylib.h"

#include "core/Config.h"
#include "entities/Player.h"

void Shop::Restock()
{
    m_items.clear();
    m_order.clear();

    m_items[Upgrade::FasterFire] = ShopItem{ "Quick Powder", "Shoot 25% faster",   10, 3 };
    m_items[Upgrade::SeaLegs] = ShopItem{ "Sea Legs",     "Move 15% faster",    10, 3 };
    m_items[Upgrade::BigShot] = ShopItem{ "Heavy Shot",   "Bullets 50% bigger", 12, 2 };
    m_items[Upgrade::ExtraLife] = ShopItem{ "Rum Ration",   "+1 life",            15, 2 };

    m_order = { Upgrade::FasterFire, Upgrade::SeaLegs, Upgrade::BigShot, Upgrade::ExtraLife };

    m_selected = 0;
    m_message.clear();
}

bool Shop::Buy(int index, Player& player)
{
    if (index < 0 || index >= static_cast<int>(m_order.size()))
        return false;

    const Upgrade key = m_order[index];
    auto it = m_items.find(key);
    if (it == m_items.end())
        return false;

    ShopItem& item = it->second;

    if (!player.SpendCoins(item.price))
    {
        m_message = "Not enough coins!";
        return false;
    }

    switch (key)
    {
    case Upgrade::ExtraLife:  
        player.AddLife();           
        break;
    case Upgrade::FasterFire: 
        player.UpgradeFireRate();   
        break;
    case Upgrade::SeaLegs:    
        player.UpgradeSpeed();      
        break;
    case Upgrade::BigShot:    
        player.UpgradeBulletSize(); 
        break;
    }
    m_message = "Bought " + item.name + "!";

    // Sold out
    --item.stock;
    if (item.stock <= 0)
    {
        m_items.erase(it);
        m_order.erase(m_order.begin() + index);
    }
    return true;
}

bool Shop::Update(Player& player)
{
    const int rows = static_cast<int>(m_order.size()) + 1;

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        m_selected = (m_selected + 1) % rows;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        m_selected = (m_selected + rows - 1) % rows;

    if (IsKeyPressed(KEY_SPACE))
        return true;

    if (IsKeyPressed(KEY_ENTER))
    {
        if (m_selected == static_cast<int>(m_order.size()))
            return true;

        Buy(m_selected, player);

        // If an item sold out, the list got shorter
        if (m_selected > static_cast<int>(m_order.size()))
            m_selected = static_cast<int>(m_order.size());
    }
    return false;
}

void Shop::Draw(const Player& player) const
{
    const int left = 24;
    const int width = Config::ScreenWidth - 48;
    const int top = Config::HUDHeight + 24;

    DrawRectangle(0, Config::HUDHeight, Config::ScreenWidth, Config::ScreenHeight - Config::HUDHeight,
        Fade(BLACK, 0.85f));

    DrawText("SHOP", left, top, 40, GOLD);
    DrawText(TextFormat("Coins: %d", player.Coins()), left + 140, top + 12, 20, GOLD);

    int y = top + 64;
    int row = 0;

    // Walk the vector for the order
    for (const Upgrade key : m_order)
    {
        const ShopItem& item = m_items.at(key);
        const int       price = item.price;

        const bool selected = (row == m_selected);
        const bool affordable = player.Coins() >= price;

        Color nameColour = GRAY;
        Color priceColour = GRAY;
        if (affordable)
        {
            nameColour = RAYWHITE;
            priceColour = GOLD;
        }

        if (selected)
            DrawRectangle(left - 8, y - 6, width + 16, 48, Fade(GOLD, 0.25f));

        DrawText(item.name.c_str(), left, y, 20, nameColour);
        const char* priceText = TextFormat("%dc", price);
        DrawText(priceText, left + width - MeasureText(priceText, 20), y, 20, priceColour);
        DrawText(TextFormat("%s   (%d left)", item.description.c_str(), item.stock), left, y + 22, 16, LIGHTGRAY);

        y += 56;
        ++row;
    }

    if (m_order.empty())
    {
        DrawText("Sold out!", left, y, 20, GRAY);
        y += 56;
    }

    // The last row: leave
    if (m_selected == static_cast<int>(m_order.size()))
        DrawRectangle(left - 8, y - 6, width + 16, 34, Fade(GOLD, 0.25f));
    DrawText("Leave shop", left, y, 20, RAYWHITE);

    if (!m_message.empty())
    {
        DrawText(m_message.c_str(), left, y + 48, 20, SKYBLUE);
    }

    DrawText("Up/Down choose   Enter buy   Space leave", left, Config::ScreenHeight - 32, 16, GRAY);
}