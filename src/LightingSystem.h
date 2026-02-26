#pragma once
#include "raylib.h"
#include "Level.h"
#include "Player.h"

class LightingSystem {
public:
    LightingSystem(Level* levelRef);
    
    void DrawOverlay(Player* player);

private:
    Level* level;
};
