#include "Scene.h"


using namespace NCL;
using namespace CSC8503;


GameObject* Scene::AddCubeToWorld(GameWorld* world, const Vector3& pos, const Vector3& size, float inverseMass)
{
    GameObject* cube = new GameObject();

    cube->tag = "Wall";
    cube->SetName("cube");

    AABBVolume* volume = new AABBVolume(size);
    cube->SetBoundingVolume((CollisionVolume*)volume);

    cube->GetTransform()
        .SetPosition(pos)
        .SetScale(size * 2.0f);

    cube->SetRenderObject(new RenderObject(&cube->GetTransform(),
        AssetManager::Instance().cubeMesh,
        AssetManager::Instance().metalTex,
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
    Vector3 offset(0, 0, 0);

    Scene::AddDefaultFloorToWorld(world, Vector3(0, -3, 0) + offset, Vector3(129, 2, 129));
    Scene::AddDefaultFloorToWorld(world, Vector3(130, -3, 0) + offset, Vector3(1, 10, 130));
    Scene::AddDefaultFloorToWorld(world, Vector3(0, -3, -130) + offset, Vector3(130, 10, 1));
    Scene::AddDefaultFloorToWorld(world, Vector3(0, -3, 130) + offset, Vector3(130, 10, 1));
    Scene::AddDefaultFloorToWorld(world, Vector3(-130, -3, 0) + offset, Vector3(1, 10, 130));
}

MeleeEnemy* Scene::AddMeleeEnemyToWorld(GameWorld* world, NavMeshGrid* nodeGrid, const Vector3& pos, const float scale, float inverseMass) {
    MeleeEnemy* e = new MeleeEnemy(nodeGrid, scale, inverseMass, pos, 500.0f, true, 30.0f, 10.0f, world);
    e->Spawn();
    world->AddGameObject(e);
    return e;
}

RangedEnemy* Scene::AddRangedEnemyToWorld(GameWorld* world, NavMeshGrid* nodeGrid, const Vector3& pos, const float scale, float inverseMass) {
   RangedEnemy* e = new RangedEnemy(nodeGrid, scale, inverseMass, pos, 100.0f, true, 15.0f, 10.0f, world);
    e->Spawn();
    world->AddGameObject(e);
    return e;
}

GhostEnemy* Scene::AddGhostEnemyToWorld(GameWorld* world, NavMeshGrid* nodeGrid, const Vector3& pos, const float scale, float inverseMass) {
   GhostEnemy* e = new GhostEnemy(nodeGrid, scale, inverseMass, pos, 50.0f, true, 15.0f, 150.0f, world);
    e->Spawn();
    world->AddGameObject(e);
    return e;
}

void Scene::InitScene(GameWorld* world) {
    std::cout << "empty scene" << std::endl;
}

void Scene::UpdateScene() {

}


void DefaultScene::InitScene(GameWorld* world) {

    //CreateRopeGroup(world);

    //GenerateWall(world);

    InitDefaultFloor(world);

    InitObstacles(world);
}

void DefaultScene2::InitScene(GameWorld* world) {

    //CreateRopeGroup(world);

    GenerateWall(world);

    InitDefaultFloor(world);
}

void EnemyTestScene::InitScene(GameWorld* world) {
    InitDefaultFloor(world);
    //GenerateWall(world);
    InitObstacles(world);
    this->navGrid = new NavMeshGrid(world);
    //add 5 of each enemy to the world, can be configured for each scene should we need to
    //player init before for loop, anything with wall tag before navgrid
    for (int i = 0; i < 5; ++i) {
        newMeleeEnemy.Invoke(AddMeleeEnemyToWorld(world, this->navGrid, Vector3(10, 3, 10), 1.5f, 0.25f));
        newRangedEnemy.Invoke(AddRangedEnemyToWorld(world, this->navGrid, Vector3(10, 3, 10), 1.0f, 0.75f));
        newGhostEnemy.Invoke(AddGhostEnemyToWorld(world, this->navGrid, Vector3(10, 3, 10), 0.5f, 1.0f));
    }
}

//scaling gameplay loop?
//more items?
//level designs
//figure out rand for negative x/z

//from default pos/cam, +x goes right, -x goes left, +z goes down, -z goes up
//coord limits are around -120, 120 for x and z
void Scene::InitObstacles(GameWorld* world) {
    int level = (std::rand() % 4) + 1;
    //int level = 4;
    if (level == 1) {
        Scene::AddCubeToWorld(world, Vector3(-120, 0, -120), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(120, 0, -120), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-120, 0, 120), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(120, 0, 120), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-100, 0, -100), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(100, 0, -100), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-100, 0, 100), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(100, 0, 100), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-80, 0, -80), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(80, 0, -80), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-80, 0, 80), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(80, 0, 80), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-60, 0, -60), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(60, 0, -60), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-60, 0, 60), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(60, 0, 60), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-40, 0, -40), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(40, 0, -40), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-40, 0, 40), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(40, 0, 40), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-20, 0, -20), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(20, 0, -20), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-20, 0, 20), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(20, 0, 20), Vector3(5, 5, 5), 0);
    }
    else if (level == 2) {
        Scene::AddCubeToWorld(world, Vector3(70, 0, 0), Vector3(60, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-70, 0, 0), Vector3(60, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(0, 0, 70), Vector3(5, 5, 60), 0);
        Scene::AddCubeToWorld(world, Vector3(0, 0, -70), Vector3(5, 5, 60), 0);
    }
    else if (level == 3) {
        Scene::AddCubeToWorld(world, Vector3(40, 9, -20), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-30, 9, 45), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-15, 9, 20), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(20, 9, -30), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(120, 9, 75), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(60, 9, -90), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(30, 9, 60), Vector3(5, 10, 5), 0);
    }
    else if (level == 4) {
        for (int i = 0; i < 50; i++) {
            int xRand = (std::rand() % 221) - 110;
            int zRand = (std::rand() % 221) - 110;
            Scene::AddCubeToWorld(world, Vector3(xRand, 0, zRand), Vector3(5, 5, 5), 0);
        }
    }
}

//scaling gameplay loop?
//more items?
//level designs
//figure out rand for negative x/z

//from default pos/cam, +x goes right, -x goes left, +z goes down, -z goes up
//coord limits are around -120, 120 for x and z
void Scene::InitObstacles(GameWorld* world) {
    int level = (std::rand() % 4) + 1;
    //int level = 4;
    if (level == 1) {
        Scene::AddCubeToWorld(world, Vector3(-120, 0, -120), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(120, 0, -120), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-120, 0, 120), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(120, 0, 120), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-100, 0, -100), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(100, 0, -100), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-100, 0, 100), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(100, 0, 100), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-80, 0, -80), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(80, 0, -80), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-80, 0, 80), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(80, 0, 80), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-60, 0, -60), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(60, 0, -60), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-60, 0, 60), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(60, 0, 60), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-40, 0, -40), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(40, 0, -40), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-40, 0, 40), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(40, 0, 40), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-20, 0, -20), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(20, 0, -20), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-20, 0, 20), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(20, 0, 20), Vector3(5, 5, 5), 0);
    }
    else if (level == 2) {
        Scene::AddCubeToWorld(world, Vector3(70, 0, 0), Vector3(60, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-70, 0, 0), Vector3(60, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(0, 0, 70), Vector3(5, 5, 60), 0);
        Scene::AddCubeToWorld(world, Vector3(0, 0, -70), Vector3(5, 5, 60), 0);
    }
    else if (level == 3) {
        Scene::AddCubeToWorld(world, Vector3(40, 9, -20), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-30, 9, 45), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-15, 9, 20), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(20, 9, -30), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(120, 9, 75), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(60, 9, -90), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(30, 9, 60), Vector3(5, 10, 5), 0);
    }
    else if (level == 4) {
        for (int i = 0; i < 50; i++) {
            int xRand = (std::rand() % 221) - 110;
            int zRand = (std::rand() % 221) - 110;
            Scene::AddCubeToWorld(world, Vector3(xRand, 0, zRand), Vector3(5, 5, 5), 0);
        }
    }
}//more items?
//level designs
//figure out rand for negative x/z

//from default pos/cam, +x goes right, -x goes left, +z goes down, -z goes up
//coord limits are around -120, 120 for x and z
void Scene::InitObstacles(GameWorld* world) {
    int level = (std::rand() % 4) + 1;
    //int level = 4;
    if (level == 1) {
        Scene::AddCubeToWorld(world, Vector3(-120, 0, -120), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(120, 0, -120), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-120, 0, 120), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(120, 0, 120), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-100, 0, -100), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(100, 0, -100), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-100, 0, 100), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(100, 0, 100), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-80, 0, -80), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(80, 0, -80), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-80, 0, 80), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(80, 0, 80), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-60, 0, -60), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(60, 0, -60), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-60, 0, 60), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(60, 0, 60), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-40, 0, -40), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(40, 0, -40), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-40, 0, 40), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(40, 0, 40), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-20, 0, -20), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(20, 0, -20), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-20, 0, 20), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(20, 0, 20), Vector3(5, 5, 5), 0);
    }
    else if (level == 2) {
        Scene::AddCubeToWorld(world, Vector3(70, 0, 0), Vector3(60, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-70, 0, 0), Vector3(60, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(0, 0, 70), Vector3(5, 5, 60), 0);
        Scene::AddCubeToWorld(world, Vector3(0, 0, -70), Vector3(5, 5, 60), 0);
    }
    else if (level == 3) {
        Scene::AddCubeToWorld(world, Vector3(40, 9, -20), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-30, 9, 45), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-15, 9, 20), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(20, 9, -30), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(120, 9, 75), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(60, 9, -90), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(30, 9, 60), Vector3(5, 10, 5), 0);
    }
    else if (level == 4) {
        for (int i = 0; i < 50; i++) {
            int xRand = (std::rand() % 221) - 110;
            int zRand = (std::rand() % 221) - 110;
            Scene::AddCubeToWorld(world, Vector3(xRand, 0, zRand), Vector3(5, 5, 5), 0);
        }
    }
}//scaling gameplay loop?
//more items?
//level designs
//figure out rand for negative x/z

//from default pos/cam, +x goes right, -x goes left, +z goes down, -z goes up
//coord limits are around -120, 120 for x and z
void Scene::InitObstacles(GameWorld* world) {
    int level = (std::rand() % 4) + 1;
    //int level = 4;
    if (level == 1) {
        Scene::AddCubeToWorld(world, Vector3(-120, 0, -120), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(120, 0, -120), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-120, 0, 120), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(120, 0, 120), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-100, 0, -100), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(100, 0, -100), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-100, 0, 100), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(100, 0, 100), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-80, 0, -80), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(80, 0, -80), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-80, 0, 80), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(80, 0, 80), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-60, 0, -60), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(60, 0, -60), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-60, 0, 60), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(60, 0, 60), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-40, 0, -40), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(40, 0, -40), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-40, 0, 40), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(40, 0, 40), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-20, 0, -20), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(20, 0, -20), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-20, 0, 20), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(20, 0, 20), Vector3(5, 5, 5), 0);
    }
    else if (level == 2) {
        Scene::AddCubeToWorld(world, Vector3(70, 0, 0), Vector3(60, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-70, 0, 0), Vector3(60, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(0, 0, 70), Vector3(5, 5, 60), 0);
        Scene::AddCubeToWorld(world, Vector3(0, 0, -70), Vector3(5, 5, 60), 0);
    }
    else if (level == 3) {
        Scene::AddCubeToWorld(world, Vector3(40, 9, -20), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-30, 9, 45), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-15, 9, 20), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(20, 9, -30), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(120, 9, 75), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(60, 9, -90), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(30, 9, 60), Vector3(5, 10, 5), 0);
    }
    else if (level == 4) {
        for (int i = 0; i < 50; i++) {
            int xRand = (std::rand() % 221) - 110;
            int zRand = (std::rand() % 221) - 110;
            Scene::AddCubeToWorld(world, Vector3(xRand, 0, zRand), Vector3(5, 5, 5), 0);
        }
    }
}
//scaling gameplay loop?
//more items?
//level designs
//figure out rand for negative x/z

//from default pos/cam, +x goes right, -x goes left, +z goes down, -z goes up
//coord limits are around -120, 120 for x and z
void Scene::InitObstacles(GameWorld* world) {
    int level = (std::rand() % 4) + 1;
    //int level = 4;
    if (level == 1) {
        Scene::AddCubeToWorld(world, Vector3(-120, 0, -120), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(120, 0, -120), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-120, 0, 120), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(120, 0, 120), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-100, 0, -100), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(100, 0, -100), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-100, 0, 100), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(100, 0, 100), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-80, 0, -80), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(80, 0, -80), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-80, 0, 80), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(80, 0, 80), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-60, 0, -60), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(60, 0, -60), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-60, 0, 60), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(60, 0, 60), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-40, 0, -40), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(40, 0, -40), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-40, 0, 40), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(40, 0, 40), Vector3(5, 5, 5), 0);

        Scene::AddCubeToWorld(world, Vector3(-20, 0, -20), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(20, 0, -20), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-20, 0, 20), Vector3(5, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(20, 0, 20), Vector3(5, 5, 5), 0);
    }
    else if (level == 2) {
        Scene::AddCubeToWorld(world, Vector3(70, 0, 0), Vector3(60, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-70, 0, 0), Vector3(60, 5, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(0, 0, 70), Vector3(5, 5, 60), 0);
        Scene::AddCubeToWorld(world, Vector3(0, 0, -70), Vector3(5, 5, 60), 0);
    }
    else if (level == 3) {
        Scene::AddCubeToWorld(world, Vector3(40, 9, -20), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-30, 9, 45), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(-15, 9, 20), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(20, 9, -30), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(120, 9, 75), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(60, 9, -90), Vector3(5, 10, 5), 0);
        Scene::AddCubeToWorld(world, Vector3(30, 9, 60), Vector3(5, 10, 5), 0);
    }
    else if (level == 4) {
        for (int i = 0; i < 50; i++) {
            int xRand = (std::rand() % 221) - 110;
            int zRand = (std::rand() % 221) - 110;
            Scene::AddCubeToWorld(world, Vector3(xRand, 0, zRand), Vector3(5, 5, 5), 0);
        }
    }
}