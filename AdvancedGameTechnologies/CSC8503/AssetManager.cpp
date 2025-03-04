#include "AssetManager.h"

#include "Animator.h"
#include "Player.h"

using namespace NCL;
using namespace CSC8503;

AssetManager& AssetManager::Instance() {
    static AssetManager instance;
    return instance;
}

void AssetManager::LoadAssets(GameTechRenderer* renderer) {
    gameTechRenderer =renderer;
    
    //load heightmap
    heightmap = new HeightMap(256, 0.1f, 50.0f);

    // load mesh resources
    cubeMesh = renderer->LoadMesh("cube.msh");
    sphereMesh = renderer->LoadMesh("sphere.msh");
    catMesh = renderer->LoadMesh("ORIGAMI_Chat.msh");
    kittenMesh = renderer->LoadMesh("Kitten.msh");
    enemyMesh = renderer->LoadMesh("Keeper.msh");
    bonusMesh = renderer->LoadMesh("19463_Kitten_Head_v1.msh");
    capsuleMesh = renderer->LoadMesh("capsule.msh");

    guardMesh = renderer->LoadMesh("Male_Guard.msh");

    terrainMesh = renderer->GenerateTerrainMesh(heightmap);


    // load texture resources
    basicTex = renderer->LoadTexture("checkerboard.png");
    woodTex = renderer->LoadTexture("wood.png");
    metalTex = renderer->LoadTexture("Metal.png");
    tilesTex = renderer->LoadTexture("Tiles.png");
    floorTex = renderer->LoadTexture("WoodFloor.png");

    // load shader resources
    basicShader = renderer->LoadShader("scene.vert", "scene.frag");
    characterShader = renderer->LoadShader("character.vert", "character.frag");

    //load animation resources 
    idle = new MeshAnimation("Idle1.anm");
    RegisterAnimation("Idle",idle);

    //load material
    guardMat = new MeshMaterial("Male_Guard.mat");

    guardMat->LoadTextures();
    for (int i = 0; i< guardMesh->GetSubMeshCount(); i++)
    {
        const MeshMaterialEntry* matEntry = guardMat ->GetMaterialForLayer(i);
        if (matEntry)
        {
            const string* filename = nullptr;

            if (matEntry->GetEntry("Diffuse", &filename))
            {
                std::cout << i << " Diffuse Texture File: " << *filename << std::endl;
                playerTex.emplace_back(renderer->LoadTexture(*filename));
            }
        }
    }
    
}

MeshAnimation* AssetManager::GetAnimation(const string& name)
{
    auto it = animationMap.find(name);
    if (it != animationMap.end()) {
        return it->second;
    }
    return nullptr;
}

void AssetManager::RegisterAnimation(const std::string& name, MeshAnimation* anim)
{
    if (anim) {
        animationMap[name] = anim;
    }
}

Texture* AssetManager::AddTexture(const string& name)
{
    Texture* tex =  gameTechRenderer->LoadTexture(name);
    return tex;
}


void AssetManager::Cleanup() {
    delete cubeMesh;
    delete sphereMesh;
    delete catMesh;
    delete kittenMesh;
    delete enemyMesh;
    delete bonusMesh;
    delete capsuleMesh;

    delete guardMesh;

    delete terrainMesh;


    delete basicTex;
    delete woodTex;
    delete metalTex;
    delete tilesTex;
    delete floorTex;

    delete basicShader;
    delete characterShader;
}
