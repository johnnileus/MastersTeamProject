For Generating a PS5 build, run the batch file and THEN open CMake GUI. Select the USE_AGC Option and set it to true and then generate. 
If there are any issues where it tells you about invalid platforms, make sure you have opened CMake GUI *after* running the batch file and generating the CMake Cache.

For generating a PC build just proceed as you would normally with CMake GUI.

This PS5 build is perfectly capable of generating either version, there are two projects as a result of this version of the project lacking many features from the final PC build

# PS5 Controls
- Left Analog Stick - Player Movement
- Right Analog Stick - Camera Movement
- Circle - Dash
- Cross - Jump
- Left / Down / Right - Switch weapons
- R2 - Shoot
- Options - Pause

# PC Controls

## Controls
- **WASD** – Move
- **Shift** – Sprint
- **Spacebar** – Jump
- **1 / 2 / 3** – Switch weapons
- **Left Mouse Button** – Shoot