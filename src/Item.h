#pragma once
#include "Entity.h"
#include "rlgl.h"
#include <cmath>

enum class ItemType {
    KEYCARD,
    OBJECTIVE,
    EXIT_ZONE
};

class Item : public Entity {
public:
    ItemType type;
    bool active;

    Item(Vector3 pos, ItemType t) : Entity(pos, 0.0f, 10.0f, YELLOW), type(t), active(true) {
        if (t == ItemType::KEYCARD) color = GOLD;
        else if (t == ItemType::OBJECTIVE) color = PURPLE;
        else if (t == ItemType::EXIT_ZONE) { color = GREEN; radius = 20.0f; }
    }

    void Update(float dt) override {
        rotation += dt * 2.0f;
    }

    void Draw() override {
        if (!active) return;
        
        if (type == ItemType::EXIT_ZONE) {
            DrawCylinder(position, radius, radius, 1.0f, 16, { 0, 255, 0, 100 });
            rlPushMatrix();
                rlTranslatef(position.x, 2.0f, position.z);
                rlRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                DrawCircle(0, 0, radius, { 0, 255, 0, 150 });
            rlPopMatrix();
            
            // Draw an exiting light beam
            DrawCylinder({position.x, 20.0f, position.z}, radius * 0.8f, radius * 0.8f, 40.0f, 16, { 0, 255, 0, 40 });
            return;
        }
        
        float bobbing = std::sin(rotation * 2.0f) * 3.0f;
        Vector3 renderPos = { position.x, 10.0f + bobbing, position.z };
        
        rlPushMatrix();
            rlTranslatef(renderPos.x, renderPos.y, renderPos.z);
            rlRotatef(rotation * RAD2DEG, 0.0f, 1.0f, 0.0f);
            
            if (type == ItemType::KEYCARD) {
                // Keycards are flat wide rectangles
                DrawCube({0,0,0}, 8.0f, 1.0f, 5.0f, color);
                DrawCubeWires({0,0,0}, 8.0f, 1.0f, 5.0f, BLACK);
            } else if (type == ItemType::OBJECTIVE) {
                // Objectives are thicker dossiers
                DrawCube({0,0,0}, 10.0f, 3.0f, 8.0f, color);
                DrawCubeWires({0,0,0}, 10.0f, 3.0f, 8.0f, BLACK);
            } else {
                DrawCube({0,0,0}, radius, radius, radius, color);
                DrawCubeWires({0,0,0}, radius, radius, radius, BLACK);
            }
            
        rlPopMatrix();
    }
};
