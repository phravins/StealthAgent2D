import os
import subprocess

def run_git_commit(msg):
    status = subprocess.run(["git", "status", "-s"], capture_output=True, text=True)
    if not status.stdout.strip():
        print(f"Skipping empty commit: {msg}")
        return
    subprocess.run(["git", "add", "."], check=True)
    subprocess.run(["git", "commit", "-m", msg], check=True)

def read_file(path):
    with open(path, 'r') as f: return f.read()

def write_file(path, content):
    with open(path, 'w') as f: f.write(content)

os.chdir('d:/GAME/StealthAgent2D')

# Start from fresh state
subprocess.run(["git", "reset", "--hard", "HEAD"], check=True)

# 1. Player.h variables
h = read_file('src/Player.h')
h = h.replace(
    "void ResolveCollision(Vector3 oldPos, Vector3 newPos);",
    "// Elastic Movement & Animation variables\n    Vector3 velocity;\n    float dashTimer;\n    float dashCooldown;\n    float currentScaleY;\n    float currentScaleXZ;\n    float walkCycle; // to bounce while moving\n    \n    void ResolveCollision(Vector3 oldPos, Vector3 newPos, bool& outHitWall);"
)
write_file('src/Player.h', h)
run_git_commit("Add elastic movement and animation variables to Player.h")


# 2. Player.cpp constructor
cpp = read_file('src/Player.cpp')
cpp = cpp.replace(
    "    hasObjective = false;\n}",
    "    hasObjective = false;\n    \n    velocity = {0};\n    dashTimer = 0.0f;\n    dashCooldown = 0.0f;\n    currentScaleY = 1.0f;\n    currentScaleXZ = 1.0f;\n    walkCycle = 0.0f;\n}"
)
write_file('src/Player.cpp', cpp)
run_git_commit("Initialize elastic variables in Player constructor")


# 3. Player.cpp elastic physics recovery & dash timer
cpp = read_file('src/Player.cpp')
cpp = cpp.replace(
    "    float currentSpeed = isCrouching ? crouchSpeed : walkSpeed;",
    "    // Elastic physics recovery\n    currentScaleY += (1.0f - currentScaleY) * 10.0f * deltaTime;\n    currentScaleXZ += (1.0f - currentScaleXZ) * 10.0f * deltaTime;\n    \n    if (dashCooldown > 0.0f) dashCooldown -= deltaTime;\n    if (dashTimer > 0.0f) dashTimer -= deltaTime;\n    \n    float currentSpeed = isCrouching ? crouchSpeed : walkSpeed;\n    if (dashTimer > 0.0f) currentSpeed = walkSpeed * 4.0f; // Dash speed multiplier"
)
write_file('src/Player.cpp', cpp)
run_git_commit("Add physics recovery and dash timers to Player Update")


# 4. Player.cpp walk cycle & dash input
cpp = read_file('src/Player.cpp')
cpp = cpp.replace(
    "        rotation = std::atan2(moveDir.z, moveDir.x);\n    }\n    \n    Vector3 oldPos = position;",
    "        if (dashTimer <= 0.0f) rotation = std::atan2(moveDir.z, moveDir.x);\n        \n        // Walk cycle animation (squash & stretch)\n        if (dashTimer <= 0.0f) {\n            walkCycle += currentSpeed * deltaTime * 0.15f;\n            currentScaleY = 1.0f + std::sin(walkCycle) * 0.15f;\n            currentScaleXZ = 1.0f - std::sin(walkCycle) * 0.05f;\n        }\n    } else {\n        walkCycle = 0.0f;\n    }\n    \n    // Dash input\n    if (IsKeyPressed(KEY_Q) && dashCooldown <= 0.0f && !isCrouching) {\n        dashTimer = 0.15f;\n        dashCooldown = 1.0f;\n        currentScaleY = 0.4f; // Squish on dash start\n        currentScaleXZ = 1.6f;\n        if (moveDir.x == 0.0f && moveDir.z == 0.0f) {\n            moveDir.x = std::cos(rotation);\n            moveDir.z = std::sin(rotation);\n        }\n    }\n    \n    Vector3 oldPos = position;"
)
write_file('src/Player.cpp', cpp)
run_git_commit("Implement walk cycle squash and stretch animation and dash input for Player")


# 5. Player.cpp momentum
cpp = read_file('src/Player.cpp')
cpp = cpp.replace(
    "    Vector3 oldPos = position;\n    Vector3 newPos = oldPos;\n    newPos.x += moveDir.x * currentSpeed * deltaTime;\n    newPos.z += moveDir.z * currentSpeed * deltaTime;\n    \n    ResolveCollision(oldPos, newPos);",
    "    // Momentum\n    if (dashTimer > 0.0f) {\n        velocity.x = moveDir.x * currentSpeed;\n        velocity.z = moveDir.z * currentSpeed;\n    } else if (moveDir.x != 0.0f || moveDir.z != 0.0f) {\n        velocity.x += (moveDir.x * currentSpeed - velocity.x) * 15.0f * deltaTime;\n        velocity.z += (moveDir.z * currentSpeed - velocity.z) * 15.0f * deltaTime;\n    } else {\n        velocity.x += (0.0f - velocity.x) * 12.0f * deltaTime;\n        velocity.z += (0.0f - velocity.z) * 12.0f * deltaTime;\n    }\n    \n    Vector3 oldPos = position;\n    Vector3 newPos = oldPos;\n    newPos.x += velocity.x * deltaTime;\n    newPos.z += velocity.z * deltaTime;\n    \n    bool hitWall = false;\n    ResolveCollision(oldPos, newPos, hitWall);\n    \n    if (hitWall) {\n        // Elastic bounce effect when hitting wall\n        currentScaleXZ = 1.4f;\n        currentScaleY = 0.6f;\n    }"
)
write_file('src/Player.cpp', cpp)
run_git_commit("Implement momentum-based physics for Player movement")


# 6. Player.cpp ResolveCollision signature and bounce logic
cpp = read_file('src/Player.cpp')
cpp = cpp.replace(
    "void Player::ResolveCollision(Vector3 oldPos, Vector3 newPos) {",
    "void Player::ResolveCollision(Vector3 oldPos, Vector3 newPos, bool& outHitWall) {"
)
cpp = cpp.replace(
    "            if (newPos.x > oldPos.x) {",
    "            outHitWall = true;\n            velocity.x *= -0.5f; // Bounce\n            if (newPos.x > oldPos.x) {"
)
cpp = cpp.replace(
    "            if (newPos.z > oldPos.z) {",
    "            outHitWall = true;\n            velocity.z *= -0.5f; // Bounce\n            if (newPos.z > oldPos.z) {"
)
cpp = cpp.replace(
    "        // Draw Torso (Cylinder)",
    "        // Apply elastic scale\n        rlScalef(currentScaleXZ, currentScaleY, currentScaleXZ);\n        \n        // Draw Torso (Cylinder)"
)
write_file('src/Player.cpp', cpp)
run_git_commit("Implement elastic wall bounce and visual scaling for Player")


# 7. Guard.h
gh = read_file('src/Guard.h')
gh = gh.replace(
    "    float investigateTimer;\n    \n    void UpdatePatrol(float dt);",
    "    float investigateTimer;\n    \n    // Animation\n    float currentScaleY;\n    float currentScaleXZ;\n    float walkCycle;\n    Vector3 velocity;\n    \n    void UpdatePatrol(float dt);"
)
write_file('src/Guard.h', gh)
run_git_commit("Add elastic animation variables to Guard.h")


# 8. Guard.cpp constructor
gcpp = read_file('src/Guard.cpp')
gcpp = gcpp.replace(
    "    investigateTimer = 0.0f;\n}",
    "    investigateTimer = 0.0f;\n    \n    currentScaleY = 1.0f;\n    currentScaleXZ = 1.0f;\n    walkCycle = 0.0f;\n    velocity = {0};\n}"
)
gcpp = gcpp.replace(
    "    if (state == GuardState::STUNNED) {\n        return;\n    }\n    \n    // Vision",
    "    if (state == GuardState::STUNNED) {\n        return;\n    }\n    \n    // Elastic physics recovery\n    currentScaleY += (1.0f - currentScaleY) * 10.0f * deltaTime;\n    currentScaleXZ += (1.0f - currentScaleXZ) * 10.0f * deltaTime;\n    \n    Vector3 oldPos = position;\n    \n    // Vision"
)
write_file('src/Guard.cpp', gcpp)
run_git_commit("Initialize elastic variables in Guard constructor and add physics recovery")


# 9. Guard.cpp walk cycle
gcpp = read_file('src/Guard.cpp')
gcpp = gcpp.replace(
    "            UpdateChase(deltaTime);\n            break;\n        default: break;\n    }\n}",
    "            UpdateChase(deltaTime);\n            break;\n        default: break;\n    }\n    \n    // Walk cycle animation\n    float distMoved = Vector3Distance(oldPos, position);\n    if (distMoved > 0.001f) {\n        float speed = distMoved / deltaTime;\n        walkCycle += speed * deltaTime * 0.15f;\n        currentScaleY = 1.0f + std::sin(walkCycle) * 0.15f;\n        currentScaleXZ = 1.0f - std::sin(walkCycle) * 0.05f;\n    } else {\n        walkCycle = 0.0f;\n    }\n}"
)
write_file('src/Guard.cpp', gcpp)
run_git_commit("Implement walk cycle squash and stretch animation for Guard")


# 10. Guard.cpp scale logic
gcpp = read_file('src/Guard.cpp')
gcpp = gcpp.replace(
    "        if (state == GuardState::STUNNED) {\n            // If stunned, lay flat on the floor\n            rlRotatef(90.0f, 0.0f, 0.0f, 1.0f);\n            rlTranslatef(-torsoHeight/2.0f, -radius, 0.0f);\n        }",
    "        if (state == GuardState::STUNNED) {\n            // If stunned, lay flat on the floor\n            rlRotatef(90.0f, 0.0f, 0.0f, 1.0f);\n            rlTranslatef(-torsoHeight/2.0f, -radius, 0.0f);\n        } else {\n            // Apply elastic scale only when not stunned\n            rlScalef(currentScaleXZ, currentScaleY, currentScaleXZ);\n        }"
)
write_file('src/Guard.cpp', gcpp)
run_git_commit("Apply elastic scale transformations to Guard rendering")
