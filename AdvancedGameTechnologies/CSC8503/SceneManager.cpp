#include "SceneManager.h"

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
