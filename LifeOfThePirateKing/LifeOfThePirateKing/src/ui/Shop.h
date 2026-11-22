#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class Player;

// What buying an item does to the player
enum class Upgrade
{
    ExtraLife,
    FasterFire,
    SeaLegs,
    BigShot
};

struct ShopItem
{
    std::string name;
    std::string description;
    int         price = 0;
    int         stock = 1;
};

class Shop
{
public:
    // Full stock for a new game
    void Restock();

    bool Update(Player& player);

    void Draw(const Player& player) const;

    // Buys the item on row `index` of the list (0 = top row)
    bool Buy(int index, Player& player);

private:
    std::unordered_map<Upgrade, ShopItem> m_items;
    std::vector<Upgrade> m_order;

    int m_selected = 0;
    std::string m_message;
};