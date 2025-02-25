
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

namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame(GameWorld& gameWorld, GameTechRendererInterface& renderer, PhysicsSystem& physics);
			~TutorialGame();

			virtual void UpdateGame(float dt);
			GameObject* AddSphereToWorld(const Vector3& position,float radius,float inverseMass,const Vector3& initialVelocity);

			void BroadcastPosition();



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
			void CreateRopeGroup();


			//networking
			GameObject* InitialiseConnectedPlayer(int id);

			NetworkManager* networkManager = new NetworkManager();
			std::vector<GameObject*> connectedPlayers;
			//
#


			//Terrain Generation
			void InitTerrain();
			
			std::vector<GameObject*> catCoins; // A list used to store all CatCoins.
			void InitCatCoins();
			
			StateGameObject* testStateObject;

			GameObject* debugSphere;
#ifdef _WIN32
			NetworkManager* networkManager = new NetworkManager();
#endif // WIN32

			

			ThirdPersonCamera* thirdPersonCam;

			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);

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
			Mesh* cubeMesh;

		};
	}
}

