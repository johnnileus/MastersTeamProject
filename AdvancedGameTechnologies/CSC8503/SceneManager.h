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
            static SceneManager& Instance() {
                static SceneManager instance;
                return instance;
            }
        

        private:
            SceneManager() {}
            SceneManager(const SceneManager&) = delete;
        };

    }
}
