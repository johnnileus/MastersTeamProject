#include "AssetManager.h"

#include "Animator.h"
#include "Player.h"
#include "GameTechRendererInterface.h"

using namespace NCL;
using namespace CSC8503;

AssetManager& AssetManager::Instance() {
    static AssetManager instance;
    return instance;
}

void AssetManager::LoadAssets(GameTechRendererInterface* renderer)  {
    //load heightmap
    heightmap = new HeightMap(256, 0.1f, 50.0f);

    // load mesh resources
	std::cout << "Loading Meshes" << std::endl;
    cubeMesh = renderer->LoadMesh("cube.msh");
	if (cubeMesh == nullptr) {
		std::cout << "Cube mesh is null" << std::endl;
	}
    //std::cout << cubeMesh << std::endl;
    //sphereMesh = renderer->LoadMesh("sphere.msh");
    //catMesh = renderer->LoadMesh("ORIGAMI_Chat.msh");
    //kittenMesh = renderer->LoadMesh("Kitten.msh");
    //enemyMesh = renderer->LoadMesh("Keeper.msh");
    //bonusMesh = renderer->LoadMesh("19463_Kitten_Head_v1.msh");
    //capsuleMesh = renderer->LoadMesh("capsule.msh");

    //guardMesh = renderer->LoadMesh("Male_Guard.msh");

    //terrainMesh = renderer->GenerateTerrainMesh(heightmap);


    // load texture resources
    basicTex = renderer->LoadTexture("checkerboard.png");
    woodTex = renderer->LoadTexture("wood.png");
    metalTex = renderer->LoadTexture("Metal.png");
    tilesTex = renderer->LoadTexture("Tiles.png");
    floorTex = renderer->LoadTexture("WoodFloor.png");

    // load shader resources
    basicShader = renderer->LoadShader("scene.vert", "scene.frag");

    //load animation resources 
    idle = new MeshAnimation("Idle1.anm");
    RegisterAnimation("Idle",idle);

    //load material
    /*guardMat = new MeshMaterial("Male_Guard.mat");

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
    }*/
    cubeMat = new MeshMaterial("Cube.mat");

    cubeMat->LoadTextures();
    for (int i = 0; i < cubeMesh->GetSubMeshCount(); i++)
    {
        const MeshMaterialEntry* matEntry = cubeMat->GetMaterialForLayer(i);
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
    std::cout << "Retrieving animation: " << name << std::endl;
    auto it = animationMap.find(name);
    if (it != animationMap.end()) {
        std::cout << "Animation found: " << it->second << std::endl;
        return it->second;
    }
    else {
        std::cout << "Animation not found: " << name << std::endl;
    }
    return nullptr;
}

void AssetManager::RegisterAnimation(const std::string& name, MeshAnimation* anim)
{
	std::cout << "Registering animation: " << name << std::endl;
    if (anim) {
        animationMap[name] = anim;
    }
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

//    delete terrainMesh;


    delete basicTex;
    delete woodTex;
    delete metalTex;
    delete tilesTex;
    delete floorTex;

    delete basicShader;
}
