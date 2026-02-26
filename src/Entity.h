#pragma once
#include "raylib.h"

class Entity {
public:
    Vector3 position;
    float rotation; // in radians (Y-axis rotation)
    float radius;
    Color color;

    Entity(Vector3 pos, float rot, float rad, Color col) 
        : position(pos), rotation(rot), radius(rad), color(col) {}

    virtual ~Entity() {}
    virtual void Update(float deltaTime) = 0;
    virtual void Draw() = 0;
};
