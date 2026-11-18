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
	constexpr int ScreenHeight = ArenaTilesY * TileSize + HUDHeight;
	constexpr int FPS = 60;
	constexpr const char* WindowTitle = "Life of The Pirate King";
	constexpr float MaxFPSTime = 0.05f;

	// Constants used for the Player
	constexpr float PlayerSpeed = TileSize * 4.5f;
	constexpr float PlayerRadius = TileSize * 0.35f;
	constexpr float FireCooldown = 0.20f;
	constexpr int StartingLives = 3;

	// Constants used for the Bullets
	constexpr float BulletSpeed = TileSize * 12.0f;
	constexpr float BulletRadius = TileSize * 0.1f;

	// Constants used for Emenies and the waves
	constexpr float SpawnInterval = 0.8f;
	constexpr float WaveBreak = 3.0f;
	constexpr float RespawnDelay = 2.0f;
	constexpr float HitFlashTime = 0.1f;

	//Constants used for the Level
	constexpr float LevelDuration = 60.0f; // how long you need to survive
}