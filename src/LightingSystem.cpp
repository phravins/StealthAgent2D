#include "LightingSystem.h"
#include "MathUtils.h"
#include "raymath.h"

LightingSystem::LightingSystem(Level* levelRef) : level(levelRef) {}

void LightingSystem::DrawOverlay(Player* player) {
    float ts = level->GetTileSize();
    float viewDist = 300.0f;
    
    // Subdivide the lighting grid into smaller chunks (e.g. 2x2 per tile)
    // to give a smoother occlusion effect closer to the player.
    int subdivisions = 2;
    float subSize = ts / subdivisions;
    
    // Render a plane grid of shadow cubes atop the ground.
    for (int z = 0; z < level->GetHeight() * subdivisions; z++) {
        for (int x = 0; x < level->GetWidth() * subdivisions; x++) {
            Vector3 center = { x * subSize + subSize / 2.0f, 0.0f, z * subSize + subSize / 2.0f };
            
            float dist = Vector3Distance(player->position, center);
            bool seen = false;
            
            if (dist < viewDist) {
                seen = MathUtils::CheckLineOfSight(level, player->position, center);
            }
            
            // Draw slightly higher than floor to prevent Z-fighting
            Vector3 shadowCenter = { center.x, 0.15f, center.z };
            
            if (!seen) {
                // Unseen area is completely dark
                DrawCube(shadowCenter, subSize, 0.2f, subSize, { 0, 0, 0, 240 });
            } else {
                // Seen area has a light vignette depending on distance
                int alpha = (int)((dist / viewDist) * 230.0f);
                if (alpha > 240) alpha = 240;
                DrawCube(shadowCenter, subSize, 0.2f, subSize, { 0, 0, 0, (unsigned char)alpha });
            }
        }
    }
}
