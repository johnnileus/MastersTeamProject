﻿#pragma once
#include <unordered_map>
#include <string>

#include "Enums.h"
#include "HeightMap.h"

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
            GameTechRenderer* gameTechRenderer;
            
            static AssetManager& Instance(); // singleton

            void LoadAssets(GameTechRenderer* renderer);
            Texture* AddTexture(const string& name);

            MeshAnimation* GetAnimation(Enums::AnimationType);
            void RegisterAnimation(Enums::AnimationType,MeshAnimation* anim);
            
            void Cleanup();

            std::vector<Texture*> FindAndLoadSubTextures(MeshMaterial* mat, const Mesh* mesh, GameTechRenderer* renderer);
            
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

        protected:
            HeightMap* heightmap;


        private:
            AssetManager() = default;
            ~AssetManager() = default;
            std::map<Enums::AnimationType, MeshAnimation*> animationMap;
        };
    }
}
