# Stealth Agent 3D

Welcome to **Stealth Agent 3D**! This is a grid-based stealth action game built in C++ using the Raylib graphics library. 

You play as a covert operative tasked with infiltrating a secure facility. You must evade patrolling guards, steal the objective folder, and escape through the extraction beam!

## How to Play

### Controls
* **WASD** or **Arrow Keys**: Move your agent around the map.
* **SHIFT (Hold)**: Sneak. Sneaking lowers your profile, making it harder for guards to spot you and allowing you to set up stealth takedowns.
* **SPACE**: Takedown. If you successfully sneak directly behind a guard, the UI will flash **TAKEDOWN READY**. Press Space to permanently stun them!
* **ESC**: Exit the game.

### Objectives
1. **Find the Keycard (Gold Box):** You may need this to unlock red security doors blocking your path.
2. **Steal the Objective (Purple Folder):** This is your primary target. You cannot leave without it.
3. **Reach the Exit Zone (Green Beam):** Once you have the objective, navigate to the green extraction point to win the game.

### Avoiding Detection
Guards actively patrol the area. You can see their field of vision represented by a dynamic cone on the floor.
* **Yellow Cone**: Guard is patrolling normally.
* **Orange Cone**: Guard heard or briefly saw something and is investigating.
* **Red Cone**: You have been spotted! The guard is chasing you and the alarm has been triggered.

## How to Run the Game (Windows)

The game is built using CMake and the MinGW C++ compiler. 

### Method 1: Running the Pre-built Executable
If the game has already been built for you, you can simply run it directly:
1. Open PowerShell or Command Prompt.
2. Navigate to the build directory:
   ```bash
   cd d:\GAME\StealthAgent2D\build
   ```
3. Run the executable:
   ```bash
   .\StealthAgent2D.exe
   ```
4. *Alternatively, you can just open your File Explorer, go to `D:\GAME\StealthAgent2D\build`, and double click on `StealthAgent2D.exe`.*

### Method 2: Building from Source
If you make changes to the code or need to recompile the game from scratch, follow these steps:
1. Open PowerShell.
2. Navigate to your project folder:
   ```bash
   cd d:\GAME\StealthAgent2D
   ```
3. Generate the build files:
   ```bash
   cmake -B build -G "MinGW Makefiles"
   ```
4. Compile the game:
   ```bash
   cmake --build build
   ```
5. Run the newly compiled game:
   ```bash
   cd build
   .\StealthAgent2D.exe
   ```

## Troubleshooting
* **Small Window / Display Issues:** The game is configured to launch in borderless native fullscreen based on your primary monitor. If it appears small, ensure your Windows display scaling isn't interfering, or press `ESC` to close it and try running the executable again while it has focus.
* **Compilation Errors:** Ensure you have CMake and a C++ compiler (like MinGW-w64) installed and added to your system's PATH.
