# Missle Defense Game

# Configuration
If running on VS Code, make sure to configure the CMake build version to GCC 5.5.0. Versions higher than this will cause compile errors.

# How to Run


## General Workflow

1. Open startup.cpp. Set the desired difficulty level.
2. Open Controller.cpp. Set the teleportationCheat variable to either true or false.
3. Configure the binInterval and safetyRad variables to vary performance
    * (default) binInterval = 2.5
    * (default) safetyRad = 4
4. Build the CMake and Run the programs through terminal
5. Press 'C' for the AI controller. Press 'space' for manual conrol of the player.
