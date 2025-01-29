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

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitGameExamples();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);

			void InitDefaultFloor();
			void AddEnemyToPoision(const Vector3& posision);
			GameObject* AddCatCoinToPosition(const Vector3& posision);
			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();
			void TestLinearMotion();
			void BridgeConstraintTest();
			void InitPlayer();
			void CreateRope(const Vector3& startPos, const Vector3& endPos, float interval);
			void CreateRopeGroup();
			
			std::vector<GameObject*> catCoins; // 用于存储所有 CatCoin 的列表
			void InitCatCoins();
			
			StateGameObject* AddStateObjectToWorld(const Vector3& position);
			StateGameObject* testStateObject;
			GameObject* AddFloorToWorld(const Vector3& position, const Vector3& size);
			//GameObject* AddSphereToWorld(const Vector3& position,float radius,float inverseMass,const Vector3& initialVelocity);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);

			GameObject* AddPlayerToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);

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

			Mesh*	capsuleMesh = nullptr;
			Mesh*	cubeMesh	= nullptr;
			Mesh*	sphereMesh	= nullptr;

			Texture*	basicTex	= nullptr;
			Texture*	woodTex     = nullptr;
			Texture*	metalTex	= nullptr;
			Texture*	tilesTex	= nullptr;
			Texture*	floorTex	= nullptr;
			Shader*		basicShader = nullptr;

			Player* player;
			std::vector<Enemy*> enemies;
			std::vector<GameObject*> floors;

			//Coursework Meshes
			Mesh*	catMesh		= nullptr;
			Mesh*	kittenMesh	= nullptr;
			Mesh*	enemyMesh	= nullptr;
			Mesh*	bonusMesh	= nullptr;

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

