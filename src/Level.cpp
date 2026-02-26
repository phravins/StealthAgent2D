#include "Level.h"
#include <iostream>

Level::Level(int width, int height, float tileSize) : width(width), height(height), tileSize(tileSize) {
    grid.resize(width * height, TILE_EMPTY);
    playerStart = { tileSize * 1.5f, 0.0f, tileSize * 1.5f }; // x, y (up), z
}

void Level::LoadTestMap() {
    const char* mapString[] = {
        "WWWWWWWWWWWWWWWWWWWWWWWWW",
        "W......W...........W....W",
        "W......W...WWWWWW..L....W",
        "W......W...W.......W....W",
        "W......L...W.......W..P.W",
        "W......W...WWWWWW..W....W",
        "W......W...........W....W",
        "W..................WWWWWW",
        "W...........WWW....W....W",
        "WWWWWWWWWW......WWWW....W",
        "W........W.........L....W",
        "W........W.........W....W",
        "W........WWWWWWW...W....W",
        "W.......................W",
        "WWWWWWWWWWWWWWWWWWWWWWWWW",
    };
    
    for (int z = 0; z < 15; z++) {
        for (int x = 0; x < 25; x++) {
            char c = mapString[z][x];
            if (c == 'W') SetTile(x, z, TILE_WALL);
            else if (c == 'L') SetTile(x, z, TILE_LOCKED_DOOR);
            else if (c == 'P') {
                playerStart = { x * tileSize + tileSize / 2.0f, 0.0f, z * tileSize + tileSize / 2.0f };
            }
        }
    }
}

void Level::Draw() {
    float wallHeight = tileSize;
    
    for (int z = 0; z < height; z++) {
        for (int x = 0; x < width; x++) {
            TileType type = GetTile(x, z);
            
            // Center position for drawing cubes
            Vector3 centerPos = { x * tileSize + tileSize/2.0f, wallHeight/2.0f, z * tileSize + tileSize/2.0f };
            
            // Checkerboard floor pattern
            Vector3 floorCenter = { x * tileSize + tileSize/2.0f, -0.1f, z * tileSize + tileSize/2.0f };
            bool isDarkSquare = ((x + z) % 2 == 0);
            Color floorColor = isDarkSquare ? Color{ 15, 15, 20, 255 } : Color{ 25, 25, 30, 255 };
            
            DrawCube(floorCenter, tileSize, 0.2f, tileSize, floorColor);
            DrawCubeWires(floorCenter, tileSize, 0.2f, tileSize, { 10, 10, 15, 255 });
            
            if (type == TILE_WALL) {
                Color wallColor = { 45, 45, 55, 255 };
                Color highlightColor = { 70, 70, 85, 255 }; // Bevel highlight
                DrawCube(centerPos, tileSize, wallHeight, tileSize, wallColor);
                DrawCubeWires(centerPos, tileSize, wallHeight, tileSize, highlightColor);
            } else if (type == TILE_LOCKED_DOOR) {
                Color doorColor = { 120, 40, 40, 255 };
                Color doorHighlight = { 180, 60, 60, 255 };
                DrawCube(centerPos, tileSize, wallHeight, tileSize, doorColor);
                DrawCubeWires(centerPos, tileSize, wallHeight, tileSize, doorHighlight);
            }
        }
    }
}

TileType Level::GetTile(int x, int z) const {
    if (x < 0 || x >= width || z < 0 || z >= height) return TILE_WALL; 
    return grid[z * width + x];
}

void Level::SetTile(int x, int z, TileType type) {
    if (x >= 0 && x < width && z >= 0 && z < height) {
        grid[z * width + x] = type;
    }
}

bool Level::IsWallAt(int gridX, int gridZ) const {
    return GetTile(gridX, gridZ) == TILE_WALL || GetTile(gridX, gridZ) == TILE_LOCKED_DOOR;
}

bool Level::IsWallAtWorld(float x, float z) const {
    int gX = (int)(x / tileSize);
    int gZ = (int)(z / tileSize);
    return IsWallAt(gX, gZ);
}
