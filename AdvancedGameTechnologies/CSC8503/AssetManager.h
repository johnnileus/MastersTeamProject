﻿#pragma once
#include <unordered_map>
#include <string>

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

            MeshAnimation* GetAnimation(const string& name);
            void RegisterAnimation(const std::string& name,MeshAnimation* anim);

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

            OGLMesh* terrainMesh = nullptr;


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
            MeshAnimation* walk;

            //Material
            MeshMaterial* guardMat;

        protected:
            HeightMap* heightmap;


        private:
            AssetManager() = default;
            ~AssetManager() = default;
            std::map<std::string, MeshAnimation*> animationMap;
        };
    }
}
