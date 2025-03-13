#include "Scene.h"

using namespace NCL;
using namespace CSC8503;


GameObject* Scene::AddCubeToWorld(GameWorld* world, const Vector3& pos, const Vector3& size, float inverseMass)
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

GameObject* Scene::AddDefaultFloorToWorld(GameWorld* world, const Vector3& position, const Vector3& size)
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

    floor->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
    floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

    floor->GetPhysicsObject()->SetInverseMass(0);
    floor->GetPhysicsObject()->InitCubeInertia();

    world->AddGameObject(floor);

    return floor;
}

GameObject* Scene::AddTerrain(GameWorld* world, const Vector3& pos, const Vector3& size) {
    GameObject* terrain = new GameObject();
    terrain->tag = "Terrain";
    terrain->SetName("terrain");
    Vector3 terrainSize = size;
    AABBVolume* terrainVolume = new AABBVolume(terrainSize);
    terrain->SetBoundingVolume((CollisionVolume*)terrainVolume);
    terrain->GetTransform().SetScale(terrainSize).SetPosition(pos);

    terrain->SetRenderObject(new RenderObject(
        &terrain->GetTransform(),
        AssetManager::Instance().sphereMesh,
        AssetManager::Instance().basicTex,
        AssetManager::Instance().basicShader));

    terrain->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
    terrain->SetPhysicsObject(new PhysicsObject(&terrain->GetTransform(), terrain->GetBoundingVolume()));

    terrain->GetPhysicsObject()->SetInverseMass(0);
    terrain->GetPhysicsObject()->InitCubeInertia();

    world->AddGameObject(terrain);

    return terrain;
}

void Scene::CreateRopeGroup(GameWorld* world) {

    Rope::AddRopeToWorld(world, Vector3(0, 0, -5), Vector3(15, 0, -5), 0.7f);
    Rope::AddRopeToWorld(world, Vector3(0, 0, 10), Vector3(15, 0, 10), 0.7f);
    Rope::AddRopeToWorld(world, Vector3(-10, 0, 30), Vector3(-10, 0, 40), 0.7f);
    Rope::AddRopeToWorld(world, Vector3(-10, 0, 30), Vector3(-5, 0, 20), 0.8f);
    Rope::AddRopeToWorld(world, Vector3(-5, 0, 50), Vector3(-10, 0, 40), 0.8f);
}

void Scene::GenerateWall(GameWorld* world)
{
    // add all walls to the list
    std::vector<GameObject*> floors;
    floors.push_back(AddDefaultFloorToWorld(world, Vector3(45, 0, 12), Vector3(6, 1, 1)));
    floors.push_back(AddDefaultFloorToWorld(world, Vector3(70, 0, 12), Vector3(6, 1, 1)));
    floors.push_back(AddDefaultFloorToWorld(world, Vector3(60, 0, 30), Vector3(8, 1, 3)));
    floors.push_back(AddDefaultFloorToWorld(world, Vector3(45, 0, 50), Vector3(8, 1, 3)));
    floors.push_back(AddDefaultFloorToWorld(world, Vector3(70, 0, 50), Vector3(3, 1, 3)));
    floors.push_back(AddDefaultFloorToWorld(world, Vector3(35, 0, 70), Vector3(9, 1, 3)));
    floors.push_back(AddDefaultFloorToWorld(world, Vector3(65, 0, 70), Vector3(8, 1, 3)));
    floors.push_back(AddDefaultFloorToWorld(world, Vector3(10, 0, 50), Vector3(4, 1, 4)));
    floors.push_back(AddDefaultFloorToWorld(world, Vector3(25, 0, 50), Vector3(2, 1, 4)));

    for (size_t i = 0; i < floors.size(); ++i) {
        floors[i]->GetRenderObject()->SetColour(Vector4(0.7, 0.7, 0.7, 1));
    }
}

void Scene::InitDefaultFloor(GameWorld* world) {
    Vector3 offset(20, 0, 20);

    Scene::AddDefaultFloorToWorld(world, Vector3(0, -3, 0) + offset, Vector3(70, 2, 70));
    Scene::AddDefaultFloorToWorld(world, Vector3(70, -3, 0) + offset, Vector3(1, 10, 70));
    Scene::AddDefaultFloorToWorld(world, Vector3(0, -3, -70) + offset, Vector3(70, 10, 1));
    Scene::AddDefaultFloorToWorld(world, Vector3(0, -3, 70) + offset, Vector3(70, 10, 1));
    Scene::AddDefaultFloorToWorld(world, Vector3(-70, -3, 0) + offset, Vector3(1, 10, 70));
}

void Scene::InitScene() {
    std::cout << "empty scene" << std::endl;
}

void Scene::UpdateScene() {

}


void DefaultScene::InitScene() {
    world = new GameWorld();
	world->ClearAndErase();

	//CreateRopeGroup(world);
	//player = Player::Instantiate(world, thirdPersonCam, Vector3(20, 0, 30));

 //   world->GetMainCamera().SetNearPlane(0.1f);
 //   world->GetMainCamera().SetFarPlane(500.0f);

 //   InitDefaultFloor(world);
 //   GenerateWall(world);

 //   doorTrigger = Door::Instantiate(world, Vector3(15, 0, 25), Vector3(20, 0, 0), Quaternion(), Quaternion());



}

