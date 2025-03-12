﻿#include "AssetManager.h"

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

    guardMesh = renderer->LoadMesh("Character_Boss.msh");
    roleMesh = renderer->LoadMesh("Role_T.msh");

    terrainMesh = (OGLMesh*)renderer->GenerateTerrainMesh(heightmap);


    //load texture resources
    //only load textures for simple models; for objects with submeshes, load them using the FindAndLoadSubTextures method.
    basicTex = renderer->LoadTexture("checkerboard.png");
    woodTex = renderer->LoadTexture("wood.png");
    metalTex = renderer->LoadTexture("Metal.png");
    tilesTex = renderer->LoadTexture("Tiles.png");
    floorTex = renderer->LoadTexture("WoodFloor.png");

    // load shader resources
    basicShader = renderer->LoadShader("scene.vert", "scene.frag");
    terrainShader = renderer->LoadShader("terrain.vert", "terrain.frag");
    characterShader = renderer->LoadShader("character.vert", "character.frag");

    //load animation resources 
    idle = new MeshAnimation("Boss_Gun_Idle.anm");
    walk = new MeshAnimation("Boss_Gun_Run.anm");
    jump = new MeshAnimation("Boss_Gun_Jump.anm");
    RegisterAnimation(AnimationType::Player_Idle,idle);
    RegisterAnimation(AnimationType::Player_Walk,walk);
    RegisterAnimation(AnimationType::Player_Jump,walk);

    //load material
    guardMat = new MeshMaterial("Character_Boss.mat");
    playerTex=FindAndLoadSubTextures(guardMat,guardMesh,renderer);
    
}

/// get the animation based on the registered animation name.
/// @param name the name that registered in the AssetManager
/// @return 
MeshAnimation* AssetManager::GetAnimation(AnimationType animation)
{
    auto it = animationMap.find(animation);
    if (it != animationMap.end()) {
        return it->second;
    }
    return nullptr;
}

void AssetManager::RegisterAnimation(AnimationType animationType, MeshAnimation* anim)
{
    if (anim) {
        animationMap[animationType] = anim;
    }
}

Texture* AssetManager::AddTexture(const string& name)
{
    Texture* tex =  gameTechRenderer->LoadTexture(name);
    return tex;
}

/// Find the textures required by the mesh based on its material information
/// @param mat the mesh's material
/// @param mesh mesh with submeshes
/// @param renderer target gameTecRenderer
std::vector<Texture*> AssetManager:: FindAndLoadSubTextures(MeshMaterial* mat, const Mesh* mesh, GameTechRenderer* renderer)
{
    std::vector<Texture*> textures;
    mat->LoadTextures();
    for (int i = 0; i < mesh->GetSubMeshCount(); i++)
    {
        const MeshMaterialEntry* matEntry = mat->GetMaterialForLayer(i);
        if (matEntry)
        {
            const string* filename = nullptr;
            if (matEntry->GetEntry("Diffuse", &filename))
            {
                std::cout << i << " Diffuse Texture File: " << *filename << std::endl;
                textures.emplace_back(renderer->LoadTexture(*filename));
            }
        }
    }
    return textures;
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
