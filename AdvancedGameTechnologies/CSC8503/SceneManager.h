#pragma once

#include <vector>
#include "GameObject.h"   // Base class for game objects (Enemy, Player, SimpleSphere, etc.)
#include "GameWorld.h"
#include "Vector.h"
#include "Quaternion.h"

namespace NCL {
    namespace CSC8503 {

        class SceneManager {
        public:
            // Singleton access
            static SceneManager& Instance();

            GameObject* AddCubeToWorld(GameWorld* world, const Vector3& pos, const Vector3& size, float inverseMass);
        
        private:
            SceneManager() {}
            SceneManager(const SceneManager&) = delete;
        };

    }
}
