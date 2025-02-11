#pragma once
#include <unordered_map>
#include <string>

#include "GameTechRenderer.h"
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
            Mesh* guard = nullptr;

            //Texture
            Texture* basicTex = nullptr;
            Texture* woodTex = nullptr;
            Texture* metalTex = nullptr;
            Texture* tilesTex = nullptr;
            Texture* floorTex = nullptr;

            //Shader
            Shader* basicShader = nullptr;

        private:
            AssetManager() = default;
            ~AssetManager() = default;
        };
    }
}
