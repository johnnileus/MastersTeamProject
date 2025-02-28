#pragma once
#include "Maths.h"
#include "MeshMaterial.h"
#include "Texture.h"

namespace NCL
{
    using namespace Maths;
    class AdvanceMeshMaterial
    {
    public:
        AdvanceMeshMaterial() = delete;
        AdvanceMeshMaterial(const std::string& fileName, const unsigned int& subMeshCount);

        std::vector<const Rendering::Texture*> GetDiffuseTextures() const
        {
            return std::vector<const Rendering::Texture*>(texturesDiffuse.begin(), texturesDiffuse.end());
        }
        std::vector<const Rendering::Texture*> GetBumpTextures() const
        {
            return std::vector<const Rendering::Texture*>(texturesBump.begin(), texturesBump.end());
        }

        std::vector<Vector4> GetSubMaterials() const
        {
            return std::vector<Vector4>(
                subMeshMaterials.begin(),
                subMeshMaterials.end()
                );
        }
        std::vector<int> GetMaterialIndex() const
        {
            return std::vector<int>(
                materialIndexes.begin(),
                materialIndexes.end()
                );
        }

        void AddMaterialIndex(int index)
        {
            materialIndexes.push_back(index);
        }
    protected:
        
        void LoadTextures(
            const std::string& entryName,
            const MeshMaterialEntry* materialEntry,
            std::vector<Rendering::Texture*>& texturesContainer
            );
        
        void LoadTextures(
            const std::string& entryName,
            const MeshMaterialEntry* materialEntry,
            std::vector<Rendering::Texture*>& texturesContainer,
            int& textureIndex
            );

        MeshMaterial* material;
        std::vector<Rendering::Texture*> texturesDiffuse;
        std::vector<Rendering::Texture*> texturesBump;

        std::vector<Vector4> subMeshMaterials;
        std::vector<int> materialIndexes;

        int materialIndex;
    
    };
}
