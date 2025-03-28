# Generating a PS5 Build
For Generating a PS5 build, run the batch file and *THEN* open CMake GUI. Select the USE_AGC Option and set it to true and then generate. 

If there are any issues where it tells you about invalid platforms, make sure you have opened CMake GUI *after* running the batch file and generating the CMake Cache.

# Generating a PC Build
For generating a PC build just proceed as you would normally with CMake.

This PS5 build is perfectly capable of generating either version, the project has been split into two folders as a result of this version of the project lacking many features from the final PC build

# PS5 Controls
- **Left Analog Stick** - Player Movement
- **Right Analog Stick** - Camera Movement
- **Circle** - Dash
- **Cross** - Jump
- **Left / Down / Right** (D-Pad) - Switch weapons
- **R2** - Shoot
- **Options** - Pause

# PC Controls
- **WASD** – Move
- **Shift** – Sprint
- **Spacebar** – Jump
- **1 / 2 / 3** – Switch weapons
- **Left Mouse Button** – Shoot

Any Artifacting in the Demo Video is a result of the Remote Viewer and not of the Actual Build