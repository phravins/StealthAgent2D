#include "Player.h"
#include "rlgl.h"
#include <cmath>

Player::Player(Vector3 pos, Level* levelRef) 
    : Entity(pos, 0.0f, 12.0f, GREEN), level(levelRef) {
    walkSpeed = 150.0f;
    crouchSpeed = 70.0f;
    isCrouching = false;
    hasKeycard = false;
    hasObjective = false;
    
    velocity = {0};
    dashTimer = 0.0f;
    dashCooldown = 0.0f;
    currentScaleY = 1.0f;
    currentScaleXZ = 1.0f;
    walkCycle = 0.0f;
}

void Player::Update(float deltaTime) {
    isCrouching = IsKeyDown(KEY_LEFT_SHIFT);
    
    // Elastic physics recovery
    currentScaleY += (1.0f - currentScaleY) * 10.0f * deltaTime;
    currentScaleXZ += (1.0f - currentScaleXZ) * 10.0f * deltaTime;
    
    if (dashCooldown > 0.0f) dashCooldown -= deltaTime;
    if (dashTimer > 0.0f) dashTimer -= deltaTime;
    
    float currentSpeed = isCrouching ? crouchSpeed : walkSpeed;
    if (dashTimer > 0.0f) currentSpeed = walkSpeed * 4.0f; // Dash speed multiplier
    
    Vector3 moveDir = { 0.0f, 0.0f, 0.0f };
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) moveDir.z -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) moveDir.z += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) moveDir.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) moveDir.x += 1.0f;
    
    if (moveDir.x != 0.0f || moveDir.z != 0.0f) {
        float length = std::sqrt(moveDir.x * moveDir.x + moveDir.z * moveDir.z);
        moveDir.x /= length;
        moveDir.z /= length;
        
        rotation = std::atan2(moveDir.z, moveDir.x);
    }
    
    Vector3 oldPos = position;
    Vector3 newPos = oldPos;
    newPos.x += moveDir.x * currentSpeed * deltaTime;
    newPos.z += moveDir.z * currentSpeed * deltaTime;
    
    ResolveCollision(oldPos, newPos);
}

void Player::ResolveCollision(Vector3 oldPos, Vector3 newPos) {
    float ts = level->GetTileSize();
    
    float checkX = newPos.x + (newPos.x > oldPos.x ? radius : -radius);
    int gX = (int)(checkX / ts);
    int gZ1 = (int)((oldPos.z - radius + 1.0f) / ts);
    int gZ2 = (int)((oldPos.z + radius - 1.0f) / ts);
    
    if (level->IsWallAt(gX, gZ1) || level->IsWallAt(gX, gZ2)) {
        if (hasKeycard) {
            if (level->GetTile(gX, gZ1) == TILE_LOCKED_DOOR) level->SetTile(gX, gZ1, TILE_EMPTY);
            if (level->GetTile(gX, gZ2) == TILE_LOCKED_DOOR) level->SetTile(gX, gZ2, TILE_EMPTY);
        }
        if (level->IsWallAt(gX, gZ1) || level->IsWallAt(gX, gZ2)) {
            if (newPos.x > oldPos.x) {
                newPos.x = gX * ts - radius - 0.1f;
            } else {
                newPos.x = (gX + 1) * ts + radius + 0.1f;
            }
        }
    }
    
    float checkZ = newPos.z + (newPos.z > oldPos.z ? radius : -radius);
    int gX1 = (int)((newPos.x - radius + 1.0f) / ts);
    int gX2 = (int)((newPos.x + radius - 1.0f) / ts);
    int gZ = (int)(checkZ / ts);
    
    if (level->IsWallAt(gX1, gZ) || level->IsWallAt(gX2, gZ)) {
        if (hasKeycard) {
            if (level->GetTile(gX1, gZ) == TILE_LOCKED_DOOR) level->SetTile(gX1, gZ, TILE_EMPTY);
            if (level->GetTile(gX2, gZ) == TILE_LOCKED_DOOR) level->SetTile(gX2, gZ, TILE_EMPTY);
        }
        if (level->IsWallAt(gX1, gZ) || level->IsWallAt(gX2, gZ)) {
            if (newPos.z > oldPos.z) {
                newPos.z = gZ * ts - radius - 0.1f;
            } else {
                newPos.z = (gZ + 1) * ts + radius + 0.1f;
            }
        }
    }
    
    position = newPos;
}

void Player::Draw() {
    Color suitColor = isCrouching ? Color{30, 40, 30, 255} : Color{40, 50, 40, 255};
    Color skinColor = { 255, 200, 150, 255 };
    
    float torsoHeight = isCrouching ? 15.0f : 25.0f;
    float headOffset = torsoHeight + 5.0f;
    
    rlPushMatrix();
        rlTranslatef(position.x, 0.0f, position.z);
        
        // Orient the player based on their rotation
        rlRotatef(rotation * RAD2DEG, 0.0f, -1.0f, 0.0f);
        
        // Draw Torso (Cylinder)
        DrawCylinder({ 0.0f, torsoHeight / 2.0f, 0.0f }, radius * 0.8f, radius * 0.9f, torsoHeight, 12, suitColor);
        DrawCylinderWires({ 0.0f, torsoHeight / 2.0f, 0.0f }, radius * 0.8f, radius * 0.9f, torsoHeight, 12, BLACK);
        
        // Draw Head (Sphere slightly offset forward if sneaking)
        float headForward = isCrouching ? 4.0f : 0.0f;
        Vector3 headPos = { headForward, headOffset, 0.0f };
        DrawSphere(headPos, radius * 0.6f, skinColor);
        
        // Draw sunglasses (classic secret agent)
        Vector3 glassesPos = { headForward + radius * 0.5f, headOffset + 1.0f, 0.0f };
        DrawCube(glassesPos, 2.0f, 3.0f, 8.0f, BLACK);
        
    rlPopMatrix();
}
