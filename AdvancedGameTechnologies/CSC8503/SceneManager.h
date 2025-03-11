#pragma once

#include <vector>

#include "Bullet.h"
#include "GameObject.h"   // Base class for game objects (Enemy, Player, SimpleSphere, etc.)
#include "GameWorld.h"
#include "Vector.h"
#include "Quaternion.h"
#include "Scene.h"

namespace NCL {
    namespace CSC8503 {

        class SceneManager {
        public:
            // Singleton access
            static SceneManager& Instance();

            GameObject* AddCubeToWorld(GameWorld* world, const Vector3& pos, const Vector3& size, float inverseMass);
            GameObject* AddDefaultFloorToWorld(GameWorld* world, const Vector3& position, const Vector3& size);
            GameObject* AddTerrain(GameWorld* world, const Vector3& pos, const Vector3& size);

            void UpdateBullets(GameWorld*world, float dt);
            void AddBullet(Bullet* bullet);

        private:
            SceneManager() {}
            SceneManager(const SceneManager&) = delete;

            std::vector<Bullet*> bullets;//store all bullets
        };

    }
}
