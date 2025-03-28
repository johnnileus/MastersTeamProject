# Generating a PS5 Build
For Generating a PS5 build, run the batch file and *THEN* open CMake GUI. Select the USE_AGC Option and set it to true and then generate. 

#SCE_MODULE
sce_module needs to be placed in \AdvancedGameTechnologies\CSC8503

#FMOD PS5
FMOD Studio API PS5 needs to be added to \AdvancedGameTechnologies\SDK and the files:
-**libfmod.prx**
-**libfmodL.prx**
in FMOD Studio API PS5\api\core\lib and the files:
-**libfmodstudio.prx**
-**libfmodstudioL.prx**
in FMOD Studio API PS5\api\studio\lib
need to be moved into **sce_module** fmod **WILL NOT** function without it

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