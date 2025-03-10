#include "AdvanceMeshMaterial.h"

#include "Assets.h"
#include "../CSC8503/AssetManager.h"
#include "../OpenGLRendering/OGLTexture.h"

NCL::AdvanceMeshMaterial::AdvanceMeshMaterial(const std::string& fileName, const unsigned int& subMeshCount)
{	
    material = new MeshMaterial(fileName);
    if (material != nullptr)
    {
        std::cout << "----------------------------------------------------------------------------\n";
        std::cout << "Loading Material for File: " << fileName << std::endl;
        for (unsigned int i = 0; i < subMeshCount; i++)
        {
            const MeshMaterialEntry* matEntry = material->GetMaterialForLayer(i);
            int diffuseIndex = -1;
            int bumpIndex = -1;
            LoadTextures("Diffuse", matEntry, texturesDiffuse, diffuseIndex);
            LoadTextures("Bump", matEntry, texturesBump, bumpIndex);
            subMeshMaterials.push_back(Vector4(diffuseIndex, bumpIndex, -1, -1));
        }
        std::cout << "----------------------------------------------------------------------------\n";
    }
}

void NCL::AdvanceMeshMaterial::LoadTextures(const std::string& entryName, const MeshMaterialEntry* materialEntry, std::vector<Texture*>& texturesContainer)
{
    const std::string* textureFileName = nullptr;
    materialEntry->GetEntry(entryName, &textureFileName);
    if (textureFileName != nullptr)
    {
        std::string filePath = *textureFileName;
        Texture* tex = AssetManager::Instance().AddTexture (*textureFileName);

        if (tex != nullptr)
        {
            std::cout << "Loaded " << entryName << " texture at path : " << filePath << ", with ID : " << dynamic_cast<OGLTexture*>(tex)->GetObjectID() << std::endl;
            texturesContainer.emplace_back(tex);
        }
    }
}

void NCL::AdvanceMeshMaterial::LoadTextures(const std::string& entryName, const MeshMaterialEntry* materialEntry, std::vector<Texture*>& texturesContainer, int& textureIndex)
{
    const std::string* textureFileName = nullptr;
    materialEntry->GetEntry(entryName, &textureFileName);
    if (textureFileName != nullptr)
    {
        std::string filePath = *textureFileName;
        Rendering::Texture* tex = AssetManager::Instance().AddTexture(*textureFileName);
        

        if (tex != nullptr)
        {
            std::cout << "Loaded " << entryName << " texture at path : " << filePath << ", with ID : " << dynamic_cast<Rendering::OGLTexture*>(tex)->GetObjectID() << std::endl;
            texturesContainer.emplace_back(tex);
        }
    }
}