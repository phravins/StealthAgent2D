#include "MathUtils.h"
#include "raymath.h"
#include <cmath>

namespace MathUtils {

    bool CheckLineOfSight(Level* level, Vector3 start, Vector3 end) {
        Vector2 s2 = { start.x, start.z };
        Vector2 e2 = { end.x, end.z };
        float dist = Vector2Distance(s2, e2);
        if (dist <= 0) return true;
        
        Vector2 dir = Vector2Normalize(Vector2Subtract(e2, s2));
        
        float stepSize = level->GetTileSize() * 0.25f; 
        
        Vector2 current = s2;
        float travelled = 0.0f;
        
        while (travelled < dist) {
            if (level->IsWallAtWorld(current.x, current.y)) {
                return false; 
            }
            current = Vector2Add(current, Vector2Scale(dir, stepSize));
            travelled += stepSize;
        }
        
        // Final check exactly at the end point
        if (level->IsWallAtWorld(e2.x, e2.y)) {
            return false;
        }
        
        return true;
    }

    bool IsPointInFOV(Vector3 viewerPos, float viewerRot, float fovAngle, float viewDist, Vector3 target) {
        Vector2 v2 = { viewerPos.x, viewerPos.z };
        Vector2 t2 = { target.x, target.z };
        
        float distSq = Vector2DistanceSqr(v2, t2);
        if (distSq > viewDist * viewDist) return false;
        
        Vector2 dirToTarget = Vector2Normalize(Vector2Subtract(t2, v2));
        float angleToTarget = std::atan2(dirToTarget.y, dirToTarget.x);
        
        float diff = angleToTarget - viewerRot;
        while (diff > PI) diff -= 2 * PI;
        while (diff < -PI) diff += 2 * PI;
        
        if (std::abs(diff) <= (fovAngle / 2.0f)) {
            return true;
        }
        return false;
    }
}
