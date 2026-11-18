#pragma once

#include <string>
#include <vector>

#include "raylib.h"

enum class Tile : unsigned char
{
	Floor,
	Wall,
	Gate
};

struct Gate
{
	Vector2 position{ 0.0f,0.0f };
	const char* name = "";
};

class Arena
{
public:
	Arena();

	// Used to build the grid from text
	void LoadFromStrings(const std::vector<std::string>& rows);

	// The coordinates of the tiles
	int Width() const 
	{
		return m_width;
	}

	int Height() const
	{
		return m_height;
	}

	bool InBounds(int x, int y) const;

	int Index(int x, int y) const
	{
		return y * m_width + x;
	}

	Tile GetTile(int x, int y) const;

	bool IsSolid(int x, int y) const;

	bool IsWall(int x, int y) const;

	// world helpers
	int WorldToTileX(float worldX) const;
	int WorldToTileY(float worldY) const;
	Vector2 TileCentre(int x, int y) const;
	Rectangle TileRect(int x, int y) const;

	bool CircleHitsSolid(Vector2 centre, float radius) const;

	bool CircleHitsWall(Vector2 centre, float radius) const;

	Vector2 PlayerStart() const
	{
		return m_playerStart;
	}

	const std::vector<Gate>& Gates() const
	{
		return m_gates;
	}

	void Draw() const;

private:
	bool CircleHits(Vector2 centre, float radius, bool gatesBlock) const;

	int m_width;
	int m_height;
	std::vector<Tile> m_tiles;

	Vector2 m_playerStart{ 0.0f, 0.0f };
	std::vector<Gate> m_gates;
};