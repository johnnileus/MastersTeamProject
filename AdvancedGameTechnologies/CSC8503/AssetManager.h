#pragma once
#include <unordered_map>
#include <string>

#include "HeightMap.h"

#include "GameTechRenderer.h"
#include "MeshAnimation.h"
#include "MeshMaterial.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "GameTechRendererInterface.h"
#include <Enums.h>

namespace NCL {
    namespace CSC8503 {
        class AssetManager {
        public:
            static AssetManager& Instance(); // singleton

            void LoadAssets(GameTechRendererInterface* renderer);

            MeshAnimation* GetAnimation(Enums::AnimationType);
            void RegisterAnimation(Enums::AnimationType, MeshAnimation* anim);
            std::vector<Texture*> FindAndLoadSubTextures(MeshMaterial* mat, const Mesh* mesh, GameTechRendererInterface* renderer);

            void Cleanup();


            // Mesh
            Mesh* capsuleMesh = nullptr;
            Mesh* cubeMesh = nullptr;
            Mesh* sphereMesh = nullptr;
            Mesh* catMesh = nullptr;
            Mesh* kittenMesh = nullptr;
            Mesh* enemyMesh = nullptr;
            Mesh* bonusMesh = nullptr;

            Mesh* guardMesh = nullptr;
            Mesh* roleMesh = nullptr;

            OGLMesh* terrainMesh = nullptr;


            //Texture
            Texture* basicTex = nullptr;
            Texture* woodTex = nullptr;
            Texture* metalTex = nullptr;
            Texture* tilesTex = nullptr;
            Texture* floorTex = nullptr;
            vector<Texture*> playerTex;
            Texture* bossTex;

            //Shader
            Shader* basicShader = nullptr;
            Shader* characterShader = nullptr;
            Shader* terrainShader = nullptr;


            //Ani
            MeshAnimation* idle;
            MeshAnimation* walk;
            MeshAnimation* jump;

            //Material
            MeshMaterial* guardMat;
			MeshMaterial* cubeMat; 

        protected:
            HeightMap* heightmap;

        private:
            AssetManager() = default;
            ~AssetManager() = default;
            GameTechRendererInterface* renderer = nullptr;
            std::map<Enums::AnimationType, MeshAnimation*> animationMap;
        };
    }
}
