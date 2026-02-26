#pragma once
#include "Entity.h"
#include "Level.h"
#include "Player.h"

class CameraObj : public Entity {
public:
    CameraObj(Vector3 pos, Level* levelRef, Player* playerRef, float startAngle, float sweepAngle);

    void Update(float deltaTime) override;
    void Draw() override;
    
    bool IsDetectingPlayer() const { return detectingPlayer; }

private:
    Level* level;
    Player* player;
    
    float baseAngle;
    float sweepAngle;
    float sweepSpeed;
    float currentSweep;
    bool sweepingLeft;
    
    float fovAngle;
    float viewDist;
    bool detectingPlayer;
};
