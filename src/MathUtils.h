#pragma once
#include "raylib.h"
#include "Level.h"

namespace MathUtils {

    // Returns true if there is a clear line of sight (no walls) between start and end
    bool CheckLineOfSight(Level* level, Vector3 start, Vector3 end);

    // Returns true if target is within the FOV cone of the viewer
    // viewerRot is in radians, fovAngle is total angle in radians
    bool IsPointInFOV(Vector3 viewerPos, float viewerRot, float fovAngle, float viewDist, Vector3 target);
}
