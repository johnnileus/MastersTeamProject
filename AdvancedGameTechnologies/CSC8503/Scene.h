#pragma once
#include <GameWorld.h>
//#include <../CSC8503/SceneManager.h>
//#include "ThirdPersonCamera.h"
//#include "Player.h"

#include "Bullet.h"
#include "GameObject.h"   // Base class for game objects (Enemy, Player, SimpleSphere, etc.)
#include "GameWorld.h"
#include "Vector.h"
#include "Quaternion.h"
#include "Scene.h"
#include "Rope.h"
#include "Player.h"
#include "Door.h"


namespace NCL {
	namespace CSC8503 {

		class Scene {
		public:

			virtual void InitScene(GameWorld* world);
			virtual void UpdateScene();

			static GameObject* AddCubeToWorld(GameWorld* world, const Vector3& pos, const Vector3& size, float inverseMass);
			static GameObject* AddDefaultFloorToWorld(GameWorld* world, const Vector3& position, const Vector3& size);
			static GameObject* AddTerrain(GameWorld* world, const Vector3& pos, const Vector3& size);
			static void InitDefaultFloor(GameWorld* world);
			static void CreateRopeGroup(GameWorld* world);
			static void GenerateWall(GameWorld* world);

		protected:

		};



		class DefaultScene : public Scene {
		public:
			void InitScene(GameWorld* world) override;
		};

		class DefaultScene2 : public Scene {
		public:
			void InitScene(GameWorld* world) override;
		};

	}
}