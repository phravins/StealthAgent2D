#include "raylib.h"
#include "raymath.h"
#include "Level.h"
#include "Player.h"
#include "Guard.h"
#include "CameraObj.h"
#include "Item.h"
#include "LightingSystem.h"
#include <vector>

enum class GameMode {
    START_SCREEN,
    PLAYING,
    GAME_OVER_WIN
};

int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);
    
    // Safely get monitor resolution for borderless fullscreen
    InitWindow(1280, 720, "Stealth Agent 3D");
    int monitor = GetCurrentMonitor();
    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);
    
    // Fallback if monitor detection fails
    if (screenWidth == 0 || screenHeight == 0) {
        screenWidth = 1280;
        screenHeight = 720;
    }
    
    SetWindowSize(screenWidth, screenHeight);
    SetWindowPosition(0, 0);

    float tileSize = 40.0f;
    Level level(25, 15, tileSize);
    level.LoadTestMap();

    Player player(level.GetPlayerStart(), &level);
    
    // We will fix Guard, CameraObj, Item shortly. For now, we will construct them using Vector3.
    Guard guard({ tileSize * 5.5f, 0.0f, tileSize * 5.5f }, &level, &player);
    guard.AddWaypoint({ tileSize * 5.5f, 0.0f, tileSize * 5.5f });
    guard.AddWaypoint({ tileSize * 15.5f, 0.0f, tileSize * 5.5f });
    guard.AddWaypoint({ tileSize * 15.5f, 0.0f, tileSize * 10.5f });
    guard.AddWaypoint({ tileSize * 5.5f, 0.0f, tileSize * 10.5f });
    
    CameraObj cam1({ tileSize * 10.5f, 0.0f, tileSize * 1.5f }, &level, &player, PI / 2.0f, PI / 2.0f);

    std::vector<Item> items;
    items.push_back(Item({ tileSize * 12.5f, 0.0f, tileSize * 2.5f }, ItemType::KEYCARD));
    items.push_back(Item({ tileSize * 2.5f, 0.0f, tileSize * 12.5f }, ItemType::OBJECTIVE));
    items.push_back(Item({ tileSize * 22.5f, 0.0f, tileSize * 4.5f }, ItemType::EXIT_ZONE));
    
    LightingSystem lightSys(&level);
    bool gameWon = false;
    
    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = { player.position.x, 250.0f, player.position.z + 180.0f }; // Lower and closer
    camera.target = player.position;
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 55.0f; // slightly wider FOV for style
    camera.projection = CAMERA_PERSPECTIVE;

    GameMode currentMode = GameMode::START_SCREEN;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if (currentMode == GameMode::START_SCREEN) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("STEALTH AGENT 3D", screenWidth/2 - 250, screenHeight/2 - 100, 60, LIGHTGRAY);
            DrawText("OBJECTIVE: Infiltrate, steal the PURPLE folder, and escape through the GREEN zone.", screenWidth/2 - 400, screenHeight/2 + 20, 20, RAYWHITE);
            DrawText("CONTROLS:", screenWidth/2 - 400, screenHeight/2 + 70, 25, LIGHTGRAY);
            DrawText("- WASD: Move", screenWidth/2 - 380, screenHeight/2 + 110, 20, RAYWHITE);
            DrawText("- SHIFT (Hold): Sneak (Reduces noise & visibility)", screenWidth/2 - 380, screenHeight/2 + 140, 20, RAYWHITE);
            DrawText("- SPACE: Takedown (Must be sneaking behind a guard)", screenWidth/2 - 380, screenHeight/2 + 170, 20, RAYWHITE);
            
            DrawText("Press [ENTER] to Start", screenWidth/2 - 150, screenHeight - 100, 30, GREEN);
            
            if (IsKeyPressed(KEY_ENTER)) {
                currentMode = GameMode::PLAYING;
            }
            EndDrawing();
            continue;
        }

        if (currentMode == GameMode::GAME_OVER_WIN) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("MISSION ACCOMPLISHED", screenWidth/2 - 250, screenHeight/2 - 20, 50, GREEN);
            DrawText("Press ESC to exit", screenWidth/2 - 120, screenHeight/2 + 50, 20, LIGHTGRAY);
            EndDrawing();
            continue;
        }

        // --- PLAYING MODE Updates ---
        player.Update(dt);
        guard.Update(dt);
        cam1.Update(dt);
        
        // Item collisions
        for (auto& item : items) {
            item.Update(dt);
            if (item.active && Vector3Distance(player.position, item.position) < player.radius + item.radius) {
                if (item.type == ItemType::KEYCARD) {
                    player.hasKeycard = true;
                    item.active = false;
                } else if (item.type == ItemType::OBJECTIVE) {
                    player.hasObjective = true;
                    item.active = false;
                } else if (item.type == ItemType::EXIT_ZONE) {
                    if (player.hasObjective) {
                        currentMode = GameMode::GAME_OVER_WIN;
                    }
                }
            }
        }
        
        // Takedowns Logic
        bool takedownReady = false;
        if (player.IsCrouching() && guard.GetState() != GuardState::STUNNED) {
            float distToGuard = Vector3Distance(player.position, guard.position);
            if (distToGuard < 60.0f) { // Increased takedown range from 40 to 60
                Vector3 dirToGuard = Vector3Normalize(Vector3Subtract(guard.position, player.position));
                Vector3 guardFaceDir = { std::cos(guard.rotation), 0.0f, std::sin(guard.rotation) };
                
                // Forgiving angle: > -0.2f means anywhere roughly behind them is fine
                if (Vector3DotProduct(dirToGuard, guardFaceDir) > -0.2f) {
                    takedownReady = true;
                    if (IsKeyPressed(KEY_SPACE)) {
                        guard.Stun();
                    }
                }
            }
        }
        
        bool isAlarm = (guard.GetState() == GuardState::CHASE) || cam1.IsDetectingPlayer();
        
        // Update Camera smoothly
        Vector3 targetCamPos = { player.position.x, 250.0f, player.position.z + 180.0f };
        camera.position.x = Lerp(camera.position.x, targetCamPos.x, dt * 5.0f);
        camera.position.y = Lerp(camera.position.y, targetCamPos.y, dt * 5.0f);
        camera.position.z = Lerp(camera.position.z, targetCamPos.z, dt * 5.0f);
        
        camera.target.x = Lerp(camera.target.x, player.position.x, dt * 8.0f);
        camera.target.y = Lerp(camera.target.y, player.position.y, dt * 8.0f);
        camera.target.z = Lerp(camera.target.z, player.position.z, dt * 8.0f);

        // --- Drawing ---
        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);

        level.Draw();
        
        for (auto& item : items) {
            item.Draw();
        }
        
        cam1.Draw();
        guard.Draw();
        player.Draw();
        
        // Lighting darkness overlay
        lightSys.DrawOverlay(&player);
        
        EndMode3D();

        // --- UI ---
        int uiHeight = 150;
        int uiY = screenHeight - uiHeight;
        
        Color uiBg = isAlarm ? Color{ 50, 0, 0, 255 } : Color{ 20, 20, 20, 255 };
        DrawRectangle(0, uiY, screenWidth, uiHeight, uiBg);
        
        const char* statusText = isAlarm ? "STATUS: ALARM TRIGGERED!" : (player.IsCrouching() ? "STATUS: SNEAKING" : "STATUS: WALKING");
        Color statusColor = isAlarm ? RED : GREEN;
        DrawText(statusText, 20, uiY + 20, 30, statusColor);
        
        if (takedownReady) {
            DrawText("TAKEDOWN READY - PRESS SPACE", 20, uiY + 60, 25, YELLOW);
        }
        
        DrawText(TextFormat("KEYCARD: %s", player.hasKeycard ? "YES" : "NO"), screenWidth - 250, uiY + 20, 20, GOLD);
        DrawText(TextFormat("OBJECTIVE: %s", player.hasObjective ? "YES" : "NO"), screenWidth - 250, uiY + 50, 20, PURPLE);
        
        DrawText("WASD: Move  |  SHIFT: Crouch  |  ESC: Exit", 20, screenHeight - 40, 15, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
