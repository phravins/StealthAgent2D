#pragma once
#include <vector>
#include "raylib.h"

// Define Tile types
enum TileType {
    TILE_EMPTY = 0,
    TILE_WALL = 1,
    TILE_LOCKED_DOOR = 2,
    TILE_KEYCARD = 3,
    TILE_OBJECTIVE = 4,
    TILE_EXIT = 5
};

class Level {
public:
    Level(int width, int height, float tileSize);
    void LoadTestMap();

    void Draw();

    bool IsWallAt(int targetX, int targetY) const;
    bool IsWallAtWorld(float x, float z) const;
    
    // Grid dimensions
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    float GetTileSize() const { return tileSize; }
    TileType GetTile(int x, int z) const;
    void SetTile(int x, int z, TileType type);
    
    // Starting position of the player
    Vector3 GetPlayerStart() const { return playerStart; }

private:
    int width;
    int height;
    float tileSize;
    Vector3 playerStart;
    std::vector<TileType> grid;
};
