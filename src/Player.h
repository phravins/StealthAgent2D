#pragma once
#include "Entity.h"
#include "Level.h"

class Player : public Entity {
public:
    Player(Vector3 pos, Level* levelRef);

    void Update(float deltaTime) override;
    void Draw() override;
    
    bool IsCrouching() const { return isCrouching; }
    
    bool hasKeycard;
    bool hasObjective;

private:
    Level* level;
    float walkSpeed;
    float crouchSpeed;
    bool isCrouching;
    
    void ResolveCollision(Vector3 oldPos, Vector3 newPos);
};
