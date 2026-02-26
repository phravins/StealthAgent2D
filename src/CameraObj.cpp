#include "CameraObj.h"
#include "MathUtils.h"
#include "raymath.h"
#include "rlgl.h"
#include <cmath>

CameraObj::CameraObj(Vector3 pos, Level* levelRef, Player* playerRef, float startAngle, float sweep) 
    : Entity(pos, startAngle, 8.0f, BLUE), level(levelRef), player(playerRef) {
    baseAngle = startAngle;
    sweepAngle = sweep;
    sweepSpeed = 0.5f; // rads per second
    currentSweep = 0.0f;
    sweepingLeft = true;
    
    fovAngle = PI / 4.0f; // 45 degrees
    viewDist = 250.0f;
    detectingPlayer = false;
}

void CameraObj::Update(float dt) {
    if (sweepingLeft) {
        currentSweep -= sweepSpeed * dt;
        if (currentSweep <= -sweepAngle / 2.0f) {
            currentSweep = -sweepAngle / 2.0f;
            sweepingLeft = false;
        }
    } else {
        currentSweep += sweepSpeed * dt;
        if (currentSweep >= sweepAngle / 2.0f) {
            currentSweep = sweepAngle / 2.0f;
            sweepingLeft = true;
        }
    }
    
    rotation = baseAngle + currentSweep;
    
    detectingPlayer = false;
    if (MathUtils::IsPointInFOV(position, rotation, fovAngle, viewDist, player->position)) {
        if (MathUtils::CheckLineOfSight(level, position, player->position)) {
            float maxDist = player->IsCrouching() ? viewDist * 0.7f : viewDist;
            if (Vector3Distance(position, player->position) <= maxDist) {
                detectingPlayer = true;
            }
        }
    }
}

void CameraObj::Draw() {
    float startA = (rotation - fovAngle * 0.5f) * RAD2DEG;
    float endA = (rotation + fovAngle * 0.5f) * RAD2DEG;
    
    Color coneAnim = detectingPlayer ? Color{ 255, 0, 0, 100 } : Color{ 0, 0, 255, 50 };
    
    rlPushMatrix();
        rlTranslatef(position.x, 0.1f, position.z);
        rlRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        DrawCircleSector({0.0f, 0.0f}, viewDist, startA, endA, 32, coneAnim);
    rlPopMatrix();
    
    float camHeight = 40.0f;
    Vector3 modelPos = { position.x, camHeight, position.z };
    
    DrawCylinder(modelPos, radius, radius, 10.0f, 8, GRAY);
    DrawSphere({modelPos.x, modelPos.y - 5.0f, modelPos.z}, radius * 0.6f, detectingPlayer ? RED : BLUE);
    
    Vector3 faceDir;
    faceDir.x = position.x + std::cos(rotation) * radius * 1.5f;
    faceDir.y = camHeight - 5.0f;
    faceDir.z = position.z + std::sin(rotation) * radius * 1.5f;
    DrawLine3D({modelPos.x, modelPos.y - 5.0f, modelPos.z}, faceDir, BLACK);
}
