﻿#include "SceneManager.h"
#include "AssetManager.h"
#include "SampleSphere.h"
#include "SphereVolume.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include "AssetManager.h"



SceneManager& SceneManager::Instance() {
    static SceneManager instance;
    return instance;
}

GameObject* SceneManager::AddCubeToWorld(GameWorld* world,const Vector3& pos, const Vector3& size, float inverseMass)
{
    GameObject* cube = new GameObject();

    AABBVolume* volume = new AABBVolume(size);
    cube->SetBoundingVolume((CollisionVolume*)volume);

    cube->GetTransform()
        .SetPosition(pos)
        .SetScale(size * 2.0f);

    cube->SetRenderObject(new RenderObject(&cube->GetTransform(),
                                           AssetManager::Instance().cubeMesh,
                                           AssetManager::Instance().basicTex,
                                           AssetManager::Instance().basicShader));
    cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

    cube->GetPhysicsObject()->SetInverseMass(inverseMass);
    cube->GetPhysicsObject()->InitCubeInertia();
	
    world->AddGameObject(cube);

    return cube;
}

GameObject* SceneManager::AddDefaultFloorToWorld(GameWorld* world, const Vector3& position, const Vector3& size)
{
    GameObject* floor = new GameObject();
    floor->tag = "Ground";
    floor->SetName("floor");
    //Vector3 floorSize = Vector3(70, 2, 70);
    Vector3 floorSize = size;
    AABBVolume* volume = new AABBVolume(floorSize);
    floor->SetBoundingVolume((CollisionVolume*)volume);
    floor->GetTransform()
        .SetScale(floorSize * 2.0f)
        .SetPosition(position);

    floor->SetRenderObject(new RenderObject(
        &floor->GetTransform(),
        AssetManager::Instance().cubeMesh,
        AssetManager::Instance().floorTex,
        AssetManager::Instance().basicShader));
    
    floor->GetRenderObject()->SetColour(Vector4(1,1,1,1));
    floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

    floor->GetPhysicsObject()->SetInverseMass(0);
    floor->GetPhysicsObject()->InitCubeInertia();

    world->AddGameObject(floor);
	
    return floor;
}

