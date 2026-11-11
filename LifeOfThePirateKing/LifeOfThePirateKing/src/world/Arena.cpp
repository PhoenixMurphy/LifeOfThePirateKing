#include "world/Arena.h"

#include <cmath>
#include <cstddef>

#include "core/Config.h"

Arena::Arena() :
	m_width(Config::ArenaTilesX),
	m_height(Config::ArenaTilesY),
	m_tiles(static_cast<std::size_t>(Config::ArenaTilesX * Config::ArenaTilesY),
		Tile::Floor)
{
}

void Arena::LoadFromStrings(const std::vector<std::string>& rows)
{
    if (static_cast<int>(rows.size()) != m_height)
    {
        TraceLog(LOG_WARNING, "ARENA: layout has %d rows when %d rows are expected",
            static_cast<int>(rows.size()), m_height);
    }

    m_gateCentres.clear();
    m_playerStart = TileCentre(m_width / 2, m_height / 2);

    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            // Missing rows and characters become floor, prevents crashing
            char c = '.';
            if (y < static_cast<int>(rows.size()) &&
                x < static_cast<int>(rows[static_cast<std::size_t>(y)].size()))
            {
                c = rows[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
            }

            Tile tile = Tile::Floor;
            switch (c)
            {
            case '#': tile = Tile::Wall; break;
            case 'G':
                tile = Tile::Gate;
                m_gateCentres.push_back(TileCentre(x, y));
                break;
            case 'P':
                m_playerStart = TileCentre(x, y); // the tile itself is floor
                break;
            default: break;
            }

            m_tiles[static_cast<std::size_t>(Index(x, y))] = tile;
        }
    }
}

bool Arena::InBounds(int x, int y) const
{
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

Tile Arena::GetTile(int x, int y) const
{
    if (!InBounds(x, y))
    {
        return Tile::Wall;
    }
    return m_tiles[static_cast<std::size_t>(Index(x, y))];
}

bool Arena::IsSolid(int x, int y) const
{
    return GetTile(x, y) != Tile::Floor;
}

int Arena::WorldToTileX(float worldX) const
{
    return static_cast<int>(std::floor(worldX / Config::TileSize));
}

int Arena::WorldToTileY(float worldY) const
{
    return static_cast<int>(std::floor((worldY - Config::HUDHeight) / Config::TileSize));
}

Vector2 Arena::TileCentre(int x, int y) const
{
    const Rectangle r = TileRect(x, y);
    return Vector2{ r.x + r.width * 0.5f, r.y + r.height * 0.5f };
}

Rectangle Arena::TileRect(int x, int y) const
{
    const float size = static_cast<float>(Config::TileSize);
    const float left = static_cast<float>(x) * size;
    const float top = static_cast<float>(Config::HUDHeight) + static_cast<float>(y) * size;
    return Rectangle{ left, top, size, size };
}

bool Arena::CircleHitsSolid(Vector2 centre, float radius) const
{
    // Checks the tiles surrounding the circle, not all the tiles
    const int minX = WorldToTileX(centre.x - radius);
    const int maxX = WorldToTileX(centre.x + radius);
    const int minY = WorldToTileY(centre.y - radius);
    const int maxY = WorldToTileY(centre.y + radius);

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            if (IsSolid(x, y) && CheckCollisionCircleRec(centre, radius, TileRect(x, y)))
                return true;
        }
    }
    return false;
}

void Arena::Draw() const
{
    const Color sandLight = { 224, 198, 142, 255 };
    const Color sandDark = { 214, 186, 128, 255 };
    const Color wall = { 112,  78,  48, 255 };
    const Color wallTop = { 140, 100,  64, 255 };
    const Color gate = { 58,  40,  30, 255 };

    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            const Rectangle r = TileRect(x, y);

            switch (GetTile(x, y))
            {
            case Tile::Floor:
                // Checkerboard so the grid is visible
                DrawRectangleRec(r, ((x + y) % 2 == 0) ? sandLight : sandDark);
                break;
            case Tile::Wall:
                DrawRectangleRec(r, wall);
                DrawRectangleRec(Rectangle{ r.x + 3, r.y + 3, r.width - 6, r.height - 10 }, wallTop);
                break;
            case Tile::Gate:
                DrawRectangleRec(r, gate);
                break;
            }
        }
    }
}