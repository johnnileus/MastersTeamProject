#include "../NCLCoreClasses/KeyboardMouseController.h"

#pragma once
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

namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);
			GameObject* AddSphereToWorld(const Vector3& position,float radius,float inverseMass,const Vector3& initialVelocity);

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);

			void InitDefaultFloor();
			void AddEnemyToPoision(const Vector3& posision);
			GameObject* AddCatCoinToPosition(const Vector3& posision);
			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();
			void TestLinearMotion();
			void InitPlayer();
			void CreateRope(const Vector3& startPos, const Vector3& endPos, float interval);
			void CreateRopeGroup();
			
			std::vector<GameObject*> catCoins; // A list used to store all CatCoins.
			void InitCatCoins();
			
			StateGameObject* testStateObject;
			GameObject* AddFloorToWorld(const Vector3& position, const Vector3& size);

			GameObject* debugSphere;
			
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
			Door* CreateDoor(const Vector3& position);
			GameObject* CreateDoorObject(const Vector3& position,const Vector3& size);

		};
	}
}

