#pragma once
#include "OGLRenderer.h"
#include "OGLShader.h"
#include "OGLTexture.h"
#include "OGLMesh.h"

#include "GameWorld.h"
#include "GameUI.h"

#include "HeightMap.h"

namespace NCL {
	namespace CSC8503 {
		class RenderObject;

		class GameTechRenderer : public OGLRenderer	{
		public:
			GameTechRenderer(GameWorld& world);
			~GameTechRenderer();

			Mesh*		LoadMesh(const std::string& name);
			Texture*	LoadTexture(const std::string& name);
			Shader*		LoadShader(const std::string& vertex, const std::string& fragment);

			//Terrain Generation
			Mesh* GenerateTerrainMesh(HeightMap* heightmap);
			Vector3 CalculateNormal(int x, int y, HeightMap* heightmap);
			void Draw(Mesh* mesh, bool multilayer);

			//Game UI
			GameUI* gameUIHandler;
			

		protected:
			void NewRenderLines();
			void NewRenderText();
			void NewRenderTextures();

			void RenderFrame()	override;

			OGLShader*		defaultShader;

			GameWorld&	gameWorld;

			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap();
			void RenderCamera(); 
			void RenderSkybox();

			void LoadSkybox();

			void SetDebugStringBufferSizes(size_t newVertCount);
			void SetDebugLineBufferSizes(size_t newVertCount);
			void PostProcessingInit();

			vector<const RenderObject*> activeObjects;

			OGLShader*  debugShader;
			OGLShader*  skyboxShader;
			OGLMesh*	skyboxMesh;
			OGLMesh*	debugTexMesh;
			GLuint		skyboxTex;

			//shadow mapping things
			OGLShader*	shadowShader;
			OGLShader*	skinnedShadowShader;
			GLuint		shadowTex;
			GLuint		shadowFBO;
			Matrix4     shadowMatrix;

			Vector4		lightColour;
			float		lightRadius;
			Vector3		lightPosition;

			//Debug data storage things
			vector<Vector3> debugLineData;

			vector<Vector3> debugTextPos;
			vector<Vector4> debugTextColours;
			vector<Vector2> debugTextUVs;

			GLuint lineVAO;
			GLuint lineVertVBO;
			size_t lineCount;

			GLuint textVAO;
			GLuint textVertVBO;
			GLuint textColourVBO;
			GLuint textTexVBO;
			size_t textCount;

			//Terrain Generation
			OGLMesh* terrainMesh;

			//PostProcessing
			GLuint postProcessFBO   = 0;
			GLuint postProcessColor = 0;
			GLuint postProcessDepth = 0;
			OGLMesh* fullScreenQuad = nullptr;
			OGLShader* postProcessShader = nullptr;

		};
	}
}

