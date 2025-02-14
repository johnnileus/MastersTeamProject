#pragma once
#include <unordered_map>
#include <string>

#include "GameTechRenderer.h"
#include "MeshAnimation.h"
#include "MeshMaterial.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif

namespace NCL {
    namespace CSC8503 {
        class AssetManager {
        public:
            static AssetManager& Instance(); // singleton

            void LoadAssets(GameTechRenderer* renderer);
            void Cleanup(); // 

            // Mesh
            Mesh* capsuleMesh = nullptr;
            Mesh* cubeMesh = nullptr;
            Mesh* sphereMesh = nullptr;
            Mesh* catMesh = nullptr;
            Mesh* kittenMesh = nullptr;
            Mesh* enemyMesh = nullptr;
            Mesh* bonusMesh = nullptr;
            Mesh* guardMesh = nullptr;

            //Texture
            Texture* basicTex = nullptr;
            Texture* woodTex = nullptr;
            Texture* metalTex = nullptr;
            Texture* tilesTex = nullptr;
            Texture* floorTex = nullptr;
            vector<Texture*> playerTex;

            //Shader
            Shader* basicShader = nullptr;

            //Ani
            MeshAnimation* idle;

            //Material
            MeshMaterial* guardMat;

        private:
            AssetManager() = default;
            ~AssetManager() = default;
        };
    }
}
