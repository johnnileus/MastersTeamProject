﻿#include "SceneManager.h"
#include "AssetManager.h"
#include "SampleSphere.h"
#include "SphereVolume.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include "DemoShootableEnemy.h"
#include "NavMeshAgent.h"
#include "MeleeEnemy.h"
#include "RangedEnemy.h"
#include "GhostEnemy.h"

SceneManager& SceneManager::Instance() {
    static SceneManager instance;
    return instance;
}


/// update all the bullets here
/// @param world 
/// @param dt 
void SceneManager::UpdateBullets(GameWorld* world,float dt) {
    for (auto it = bullets.begin(); it != bullets.end(); ) {
        Bullet* bullet = *it;
        bullet->Update(dt);

        if (bullet->IsDead()) {
            world->RemoveGameObject((GameObject*)bullet);
            it = bullets.erase(it);
        } else {
            ++it;
        }
    }
}

void SceneManager::AddBullet(Bullet* bullet)
{
    bullets.push_back(bullet);
}


MeleeEnemy* SceneManager::AddEnemyToWorld(GameWorld* world,NavMeshGrid* nodeGrid, const Vector3& pos, const float scale, float inverseMass) {
    MeleeEnemy* e = new MeleeEnemy(nodeGrid, scale, inverseMass, pos, 100.0f, true, 15.0f, 10.0f, world);
    world->AddGameObject(e);
    return e;
}

void SceneManager::InitScenes() {
    DefaultScene* defaultScene = new DefaultScene();
    defaultScene->InitScene();

}

void SceneManager::SwitchScene(string newScene) {
    //world->GetMainCamera().SetController(controller);
//thirdPersonCam = new ThirdPersonCamera(&world->GetMainCamera(), controller);
    //if (thirdPersonCam)
    //{
    //    thirdPersonCam->SetPitch(0.0f);
    //    thirdPersonCam->SetYaw(0.0f);
    //}


}