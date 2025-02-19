#include "AssetManager.h"
#include "TextureLoader.h"

using namespace NCL;
using namespace CSC8503;

AssetManager& AssetManager::Instance() {
    static AssetManager instance;
    return instance;
}

void AssetManager::LoadAssets(GameTechRenderer* renderer) {
    // load mesh resources
    cubeMesh = renderer->LoadMesh("cube.msh");
    sphereMesh = renderer->LoadMesh("sphere.msh");
    catMesh = renderer->LoadMesh("ORIGAMI_Chat.msh");
    kittenMesh = renderer->LoadMesh("Kitten.msh");
    enemyMesh = renderer->LoadMesh("Keeper.msh");
    bonusMesh = renderer->LoadMesh("19463_Kitten_Head_v1.msh");
    capsuleMesh = renderer->LoadMesh("capsule.msh");
    terrainMesh = renderer->GenerateTerrainMesh(heightmap);

    // load texture resources
    basicTex = renderer->LoadTexture("checkerboard.png");
    woodTex = renderer->LoadTexture("wood.png");
    metalTex = renderer->LoadTexture("Metal.png");
    tilesTex = renderer->LoadTexture("Tiles.png");
    floorTex = renderer->LoadTexture("WoodFloor.png");

    // load shader resources
    basicShader = renderer->LoadShader("scene.vert", "scene.frag");
}

void AssetManager::Cleanup() {
    delete cubeMesh;
    delete sphereMesh;
    delete catMesh;
    delete kittenMesh;
    delete enemyMesh;
    delete bonusMesh;
    delete capsuleMesh;
    delete terrainMesh;

    delete basicTex;
    delete woodTex;
    delete metalTex;
    delete tilesTex;
    delete floorTex;

    delete basicShader;
}
