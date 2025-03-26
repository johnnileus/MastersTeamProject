
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
#include "GameTechRendererInterface.h"

#include "Pistol.h"

#include "NavMeshGrid.h"
#include "NavMeshAgent.h"
#include "DemoShootableEnemy.h"

namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame(GameWorld& gameWorld, GameTechRendererInterface& renderer, PhysicsSystem& physics);
			~TutorialGame();

			virtual void UpdateGame(float dt);

			void BroadcastPosition();
			void SendTransform();
			void UpdateConnectedPlayer(int id, Vector3 pos, Quaternion rot);
			void ToggleCursor();
			void TogglePaused() { gamePaused = !gamePaused; }
			bool IsGamePaused() { return gamePaused; }
			bool GetCursorLocked() { return cursorLocked; }

#ifdef _WIN32
			//called by GameServer when player connects
			GameObject* InitialiseConnectedPlayerObject(int id);
			GameObject** GetConnectedPlayerObjects();
			void BroadcastPositions();

			
			int GetID() { return networkManager->GetID(); }
			

			void UpdateTransformFromServer(Vector3 pos, Quaternion rot);
#endif // _WIN32

		
			

		protected:
			void InitialiseAssets();

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
			float timer = 180;

			//Transitions
			void Transition();

			//networking

#ifdef _WIN32
			NetworkManager* networkManager = new NetworkManager();
#endif // _WIN32
			GameObject* connectedPlayers[8];

			//SceneManager
			SceneManager* sceneManager;

			std::vector<GameObject*> catCoins; // A list used to store all CatCoins.
			void InitCatCoins();
			
			StateGameObject* testStateObject;

			GameObject* debugSphere;
#ifdef _WIN32
			NetworkManager* networkManager = new NetworkManager();
#endif // WIN32

			ThirdPersonCamera* thirdPersonCam;


			GameWorld& world;
			GameTechRendererInterface& renderer;
			PhysicsSystem& physics;

#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			//GameTechRenderer* renderer;
#endif
			//PhysicsSystem*		physics;
			//GameWorld*			world;

#ifdef USEAGC
			NCL::PS5::PS5Controller controller;
#else
			KeyboardMouseController controller;
#endif // _WIN32
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
			void GenerateWall();
			void SetWallColour();

			void DisplayPathfinding(); // display navigation path
			std::vector<Vector3> testNodes; // save nodes

			void ReloadLevel();

			Door* doorTrigger;
			Mesh* cubeMesh;

			NavMeshGrid* navGrid;
			NavMeshAgent* navMeshAgent;
			std::vector<NavMeshAgent*> enemyList;

			int EnemyFrameCountMax;
			int EnemyFrameCount;
		};
	}
}

