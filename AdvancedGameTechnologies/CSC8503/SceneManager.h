#pragma once

#include <vector>
#include <unordered_map>
#include <string>


#include "Bullet.h"
#include "GameObject.h"   // Base class for game objects (Enemy, Player, SimpleSphere, etc.)
#include "GameWorld.h"
#include "Vector.h"
#include "Quaternion.h"
#include "DemoShootableEnemy.h"
#include "Scene.h"
#include "Rope.h"

namespace NCL {
    namespace CSC8503 {


        class SceneManager {
        public:
            SceneManager() {}
            SceneManager(const SceneManager&) = delete;

            
            // Singleton access
            static SceneManager& Instance();


            void UpdateBullets(GameWorld*world, float dt);
            void AddBullet(Bullet* bullet);
          
            DemoShootableEnemy* AddEnemyToWorld(GameWorld* world, const Vector3& pos, const float scale, float inverseMass);

            void InitScenes();
            void SwitchScene(string newScene, GameWorld* world);
            
            unordered_map<string, Scene*> scenes;


        private:

            std::vector<Bullet*> bullets;//store all bullets
        };

    }
}
