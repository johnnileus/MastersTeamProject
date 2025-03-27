
#pragma once
#include "../NCLCoreClasses/KeyboardMouseController.h"
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "PhysicsSystem.h"
#include "Player.h"
#include "Enemy.h"
#include <vector>

#include "CatCoin.h"
#include "Door.h"
#include "StateGameObject.h"
#include "AssetManager.h"
#include "ThirdPersonCamera.h"

#include "GameNet.h"

#include "Constants.h"
#include "GameWorld.h"
#include "NavigationGrid.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "Rope.h"
#include "SampleSphere.h"
#include "SceneManager.h"

#include "HeightMap.h"

#include "Pistol.h"

#include "NavMeshGrid.h"
#include "NavMeshAgent.h"
#include "DemoShootableEnemy.h"

#include "MeleeEnemy.h"
#include "RangedEnemy.h"
#include "GhostEnemy.h"

#include "PassiveItem.h"


namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			void InitScene(string name);

			virtual void UpdateGame(float dt);

			void BroadcastPosition();
			void SendTransform();
			void UpdateConnectedPlayer(int id, Vector3 pos, Quaternion rot);

			//called by GameServer when player connects
			GameObject* InitialiseConnectedPlayerObject(int id);
			GameObject** GetConnectedPlayerObjects();
			void BroadcastPositions();

			void ToggleCursor();
			void TogglePaused() { gamePaused = !gamePaused; }
			int GetID() { return networkManager->GetID(); }
			bool IsGamePaused() { return gamePaused; }

			bool GetCursorLocked() { return cursorLocked; }

			Player* getPlayer() { return player; }

		protected:

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			void InitDefaultFloor();
			void CreateRopeGroup();

			void InitNavigationTestLevel();
			void InitNavGrid();
			void InitEnemies();
			void UpdateEnemies(float dt);

			//Terrain Generation
			void InitTerrain();

			//Timer
			float timerSecs = 0;
			float timerMins = 0;

			//Items
			void InitItems();
			std::vector<PassiveItem*> itemList;

			//networking
			NetworkManager* networkManager = new NetworkManager();
			GameObject* connectedPlayers[8];

			//SceneManager
			SceneManager* sceneManager;

			std::vector<GameObject*> catCoins; // A list used to store all CatCoins.
			void InitCatCoins();
			
			StateGameObject* testStateObject;

			GameObject* debugSphere;

			ThirdPersonCamera* thirdPersonCam;


#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem*		physics;
			GameWorld*			world;


			KeyboardMouseController controller;

			bool cursorLocked;
			bool gamePaused = false;
			bool useGravity;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;
			

			Player* player;
			std::vector<Enemy*> enemies;
			std::vector<GameObject*> floors;

			int currentFrame;
			float frameTime;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 15, 25);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}


			GameObject* objClosest = nullptr;
			void SetWallColour();

			void DisplayPathfinding(); // display navigation path
			std::vector<Vector3> testNodes; // save nodes

			void ReloadLevel();

			Door* doorTrigger;

			NavMeshGrid* navGrid;
			NavMeshAgent* navMeshAgent;
			std::vector<MeleeEnemy*> meleeEnemyList;
			std::vector<RangedEnemy*> rangedEnemyList;
			std::vector<GhostEnemy*> ghostEnemyList;

			int enemyFrameCount;
			int meleeEnemyFrameCountMax;
			int meleeEnemyFrameCount;
			int rangedEnemyFrameCountMax;
			int rangedEnemyFrameCount;
			int ghostEnemyFrameCountMax;
			int ghostEnemyFrameCount;
			std::vector<DemoShootableEnemy*> enemyList;
			float bgmVolume = 0.5f;

		};
	}
}

