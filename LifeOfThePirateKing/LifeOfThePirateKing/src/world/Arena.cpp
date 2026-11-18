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

static int EdgeOf(int x, int y, int width, int  height)
{
    if (y == 0)
    {
        return 0;
    }

    if (x == width - 1)
    {
        return 1;
    }

    if (y == height - 1)
    {
        return 2;
    }

    if (x == 0)
    {
        return 3;
    }

    return -1;
}

void Arena::LoadFromStrings(const std::vector<std::string>& rows)
{
    if (static_cast<int>(rows.size()) != m_height)
    {
        TraceLog(LOG_WARNING, "ARENA: layout has %d rows when %d rows are expected",
            static_cast<int>(rows.size()), m_height);
    }

    m_gates.clear();
    m_playerStart = TileCentre(m_width / 2, m_height / 2);
    Vector2 gateSum[4] = {};
    int gateCount[4] = {};

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
            {
                tile = Tile::Gate;
                const int edge = EdgeOf(x, y, m_width, m_height);
                if (edge >= 0)
                {
                    const Vector2 centre = TileCentre(x, y);
                    gateSum[edge].x += centre.x;
                    gateSum[edge].y += centre.y;
                    ++gateCount[edge];
                }
            }
                break;
            case 'P':
                m_playerStart = TileCentre(x, y); // the tile itself is floor
                break;
            default: break;
            }

            m_tiles[static_cast<std::size_t>(Index(x, y))] = tile;
        }
    }

    const char* edgeNames[4] = { "North", "East", "South", "West" };
    for (int edge = 0; edge < 4; ++edge)
    {
        if (gateCount[edge] == 0)
        {
            continue;
        }

        const float numberOfTiles = static_cast<float>(gateCount[edge]);
        Gate gate;
        gate.position = Vector2{ gateSum[edge].x / numberOfTiles, gateSum[edge].y / numberOfTiles };
        gate.name = edgeNames[edge];
        m_gates.push_back(gate);
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

bool Arena::IsWall(int x, int y) const
{
    return GetTile(x, y) == Tile::Wall;
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
    return CircleHits(centre, radius, true);
}

bool Arena::CircleHitsWall(Vector2 centre, float radius) const
{
    return CircleHits(centre, radius, false);
}

bool Arena::CircleHits(Vector2 centre, float radius, bool gatesBlock) const
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
            bool blocks;
            if (gatesBlock)
            {
                blocks = IsSolid(x, y);
            }
            else
            {
                blocks = IsWall(x, y);
            }
            if (blocks && CheckCollisionCircleRec(centre, radius, TileRect(x, y)))
            {
                return true;
            }
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