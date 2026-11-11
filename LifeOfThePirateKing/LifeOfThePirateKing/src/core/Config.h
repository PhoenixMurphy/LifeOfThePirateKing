#pragma once

namespace Config
{
	// Constants used for the Arena
	constexpr int ArenaTilesX = 16;
	constexpr int ArenaTilesY = 16;
	constexpr int TileSize = 48;

	// Constants used for the Window
	constexpr int HUDHeight = 64;
	constexpr int ScreenWidth = ArenaTilesX * TileSize;
	constexpr int ScreenHeight = ArenaTilesY * TileSize;
	constexpr int FPS = 60;
	constexpr const char* WindowTitle = "Life of The Pirate King";
	constexpr float MaxFPSTime = 0.05f;

	// Constants used for the Player
	constexpr float PlayerSpeed = 180.0f;
	constexpr float PlayerRadius = 14.0f;
	constexpr float FireCooldown = 0.20f;
	constexpr int StartingLives = 3;

	// Constants used for the Bullets
	constexpr float BulletSpeed = 480.0f;
	constexpr float BulletRadius = 4.0f;

	//Constants used for the Level
	constexpr float LevelDuration = 60.0f; // how long you need to survive
}