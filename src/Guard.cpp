#include "Guard.h"
#include "MathUtils.h"
#include "raymath.h"
#include "rlgl.h"
#include <cmath>

Guard::Guard(Vector3 pos, Level* levelRef, Player* playerRef) 
    : Entity(pos, 0.0f, 12.0f, RED), level(levelRef), player(playerRef) {
    state = GuardState::PATROL;
    walkSpeed = 60.0f;
    runSpeed = 130.0f;
    fovAngle = PI / 3.0f; 
    viewDist = 200.0f;
    currentWaypointIdx = 0;
    waitTimer = 0.0f;
    investigateTimer = 0.0f;
    
    currentScaleY = 1.0f;
    currentScaleXZ = 1.0f;
    walkCycle = 0.0f;
    velocity = {0};
}

void Guard::AddWaypoint(Vector3 wp) {
    waypoints.push_back(wp);
}

void Guard::Update(float deltaTime) {
    if (state == GuardState::STUNNED) {
        return;
    }
    
    // Elastic physics recovery
    currentScaleY += (1.0f - currentScaleY) * 10.0f * deltaTime;
    currentScaleXZ += (1.0f - currentScaleXZ) * 10.0f * deltaTime;
    
    Vector3 oldPos = position;
    
    // Vision
    if (SeePlayer()) {
        state = GuardState::CHASE;
    } else if (state == GuardState::CHASE) {
        state = GuardState::INVESTIGATE;
        investigateTarget = player->position;
        investigateTimer = 3.0f;
    }
    
    switch (state) {
        case GuardState::PATROL:
            UpdatePatrol(deltaTime);
            break;
        case GuardState::INVESTIGATE:
            UpdateInvestigate(deltaTime);
            break;
        case GuardState::CHASE:
            UpdateChase(deltaTime);
            break;
        default: break;
    }
    
    // Walk cycle animation
    float distMoved = Vector3Distance(oldPos, position);
    if (distMoved > 0.001f) {
        float speed = distMoved / deltaTime;
        walkCycle += speed * deltaTime * 0.15f;
        currentScaleY = 1.0f + std::sin(walkCycle) * 0.15f;
        currentScaleXZ = 1.0f - std::sin(walkCycle) * 0.05f;
    } else {
        walkCycle = 0.0f;
    }
}

void Guard::UpdatePatrol(float dt) {
    if (waypoints.empty()) return;
    
    if (waitTimer > 0.0f) {
        waitTimer -= dt;
        return;
    }
    
    Vector3 target = waypoints[currentWaypointIdx];
    float dist = Vector3Distance(position, target);
    
    if (dist < 5.0f) {
        currentWaypointIdx = (currentWaypointIdx + 1) % waypoints.size();
        waitTimer = 1.0f;
        return;
    }
    
    Vector3 dir = Vector3Normalize(Vector3Subtract(target, position));
    rotation = std::atan2(dir.z, dir.x);
    
    position.x += dir.x * walkSpeed * dt;
    position.z += dir.z * walkSpeed * dt;
}

void Guard::UpdateInvestigate(float dt) {
    float dist = Vector3Distance(position, investigateTarget);
    if (dist > 5.0f) {
        Vector3 dir = Vector3Normalize(Vector3Subtract(investigateTarget, position));
        rotation = std::atan2(dir.z, dir.x);
        position.x += dir.x * runSpeed * dt;
        position.z += dir.z * runSpeed * dt;
    } else {
        rotation += PI * 0.5f * dt;
        investigateTimer -= dt;
        if (investigateTimer <= 0.0f) {
            state = GuardState::PATROL;
        }
    }
}

void Guard::UpdateChase(float dt) {
    Vector3 target = player->position;
    Vector3 dir = Vector3Normalize(Vector3Subtract(target, position));
    rotation = std::atan2(dir.z, dir.x);
    
    position.x += dir.x * runSpeed * dt;
    position.z += dir.z * runSpeed * dt;
    
    float ts = level->GetTileSize();
    float checkX = position.x + (dir.x > 0 ? radius : -radius);
    if (level->IsWallAtWorld(checkX, position.z)) {
        position.x -= dir.x * runSpeed * dt; 
    }
    float checkZ = position.z + (dir.z > 0 ? radius : -radius);
    if (level->IsWallAtWorld(position.x, checkZ)) {
        position.z -= dir.z * runSpeed * dt; 
    }
}

bool Guard::SeePlayer() {
    if (MathUtils::IsPointInFOV(position, rotation, fovAngle, viewDist, player->position)) {
        if (MathUtils::CheckLineOfSight(level, position, player->position)) {
            float maxDist = player->IsCrouching() ? viewDist * 0.5f : viewDist;
            if (Vector3Distance(position, player->position) <= maxDist) {
                return true;
            }
        }
    }
    return false;
}

void Guard::DrawVisionCone() {
    if (state == GuardState::STUNNED) return;
    Color coneAnim = { 255, 255, 0, 50 };
    if (state == GuardState::INVESTIGATE) coneAnim = { 255, 165, 0, 80 };
    else if (state == GuardState::CHASE) coneAnim = { 255, 0, 0, 80 };
    
    rlPushMatrix();
        rlTranslatef(position.x, 0.1f, position.z);
        rlRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotate flat against floor
        // DrawCircleSector takes startAngle to endAngle
        float startAngle = (rotation - fovAngle * 0.5f) * RAD2DEG;
        float endAngle = (rotation + fovAngle * 0.5f) * RAD2DEG;
        DrawCircleSector({0.0f, 0.0f}, viewDist, startAngle, endAngle, 32, coneAnim);
    rlPopMatrix();
}

void Guard::Draw() {
    DrawVisionCone();
    
    Color suitColor = color;
    Color skinColor = { 255, 200, 150, 255 };
    if (state == GuardState::STUNNED) {
        suitColor = GRAY;
        skinColor = LIGHTGRAY;
    }
    
    float torsoHeight = 25.0f;
    float headOffset = torsoHeight + 5.0f;
    
    rlPushMatrix();
        rlTranslatef(position.x, 0.0f, position.z);
        
        // Orient the guard based on their rotation
        rlRotatef(rotation * RAD2DEG, 0.0f, -1.0f, 0.0f);
        
        if (state == GuardState::STUNNED) {
            // If stunned, lay flat on the floor
            rlRotatef(90.0f, 0.0f, 0.0f, 1.0f);
            rlTranslatef(-torsoHeight/2.0f, -radius, 0.0f);
        }
        
        // Draw Torso (Cylinder)
        DrawCylinder({ 0.0f, torsoHeight / 2.0f, 0.0f }, radius * 0.8f, radius * 0.9f, torsoHeight, 12, suitColor);
        DrawCylinderWires({ 0.0f, torsoHeight / 2.0f, 0.0f }, radius * 0.8f, radius * 0.9f, torsoHeight, 12, BLACK);
        
        // Draw Head
        Vector3 headPos = { 0.0f, headOffset, 0.0f };
        DrawSphere(headPos, radius * 0.6f, skinColor);
        
        // Draw sunglasses (classic guard)
        Vector3 glassesPos = { radius * 0.5f, headOffset + 1.0f, 0.0f };
        DrawCube(glassesPos, 2.0f, 3.0f, 8.0f, BLACK);
        
        // Draw a tiny hat
        Vector3 hatPos = { 0.0f, headOffset + radius * 0.6f, 0.0f };
        DrawCylinder(hatPos, radius * 0.65f, radius * 0.65f, 2.0f, 8, DARKGRAY);
        DrawCylinder({ 0.0f, headOffset + radius * 0.6f + 2.0f, 0.0f }, radius * 0.4f, radius * 0.4f, 4.0f, 8, DARKGRAY);
        
    rlPopMatrix();
}
