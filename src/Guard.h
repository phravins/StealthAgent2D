#pragma once
#include "Entity.h"
#include "Level.h"
#include "Player.h"
#include <vector>

enum class GuardState {
    PATROL,
    INVESTIGATE,
    CHASE,
    STUNNED
};

class Guard : public Entity {
public:
    Guard(Vector3 pos, Level* levelRef, Player* playerRef);

    void Update(float deltaTime) override;
    void Draw() override;
    
    void AddWaypoint(Vector3 wp);
    GuardState GetState() const { return state; }
    void Stun() { state = GuardState::STUNNED; }

private:
    Level* level;
    Player* player;
    
    GuardState state;
    
    float walkSpeed;
    float runSpeed;
    
    // Vision
    float fovAngle;
    float viewDist;
    
    // Patrol
    std::vector<Vector3> waypoints;
    int currentWaypointIdx;
    float waitTimer;
    
    // Investigate
    Vector3 investigateTarget;
    float investigateTimer;
    
    void UpdatePatrol(float dt);
    void UpdateInvestigate(float dt);
    void UpdateChase(float dt);
    
    bool SeePlayer();
    void DrawVisionCone();
};
