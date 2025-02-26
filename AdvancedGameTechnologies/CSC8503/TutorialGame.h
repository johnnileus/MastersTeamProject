
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


namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);
			GameObject* AddSphereToWorld(const Vector3& position,float radius,float inverseMass,const Vector3& initialVelocity);

			void BroadcastPosition();

			//called by GameServer when player connects
			GameObject* InitialiseConnectedPlayer(int id);

			void UpdateTransformFromServer(Vector3 pos, Quaternion rot);


		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);

			void InitDefaultFloor();
			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();
			void TestLinearMotion();
			void InitPlayer();
			void CreateRopeGroup();


			//Terrain Generation
			void InitTerrain();

			//networking

			NetworkManager* networkManager = new NetworkManager();
			std::vector<GameObject*> connectedPlayers;


			std::vector<GameObject*> catCoins; // A list used to store all CatCoins.
			void InitCatCoins();
			
			StateGameObject* testStateObject;

			GameObject* debugSphere;



			ThirdPersonCamera* thirdPersonCam;

			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);

#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem*		physics;
			GameWorld*			world;


			KeyboardMouseController controller;

			bool useGravity;
			bool inSelectionMode;

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
			
			bool allCoinsCollected;   // Whether all coins have been collected
			void CheckCoinsCollected(); // Check if all coins have been collected

			void ShowSuccessMessage();  // Display success message

			void ReloadLevel();

			Door* doorTrigger;

		};
	}
}

