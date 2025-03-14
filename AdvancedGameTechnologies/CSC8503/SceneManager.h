﻿#pragma once

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

            GameObject* AddCubeToWorld(GameWorld* world, const Vector3& pos, const Vector3& size, float inverseMass);
            GameObject* AddDefaultFloorToWorld(GameWorld* world, const Vector3& position, const Vector3& size);
            GameObject* AddTerrain(GameWorld* world, const Vector3& pos, const Vector3& size);
            void CreateRopeGroup(GameWorld* world);
            void GenerateWall(GameWorld* world);

            void UpdateBullets(GameWorld*world, float dt);
            void AddBullet(Bullet* bullet);
          
            DemoShootableEnemy* AddEnemyToWorld(GameWorld* world, const Vector3& pos, const float scale, float inverseMass);

            void InitScenes();
            void SwitchScene(string newScene);
            
            unordered_map<string, Scene*> scenes;
            Scene* activeScene;

        private:

            std::vector<Bullet*> bullets;//store all bullets
        };

    }
}
