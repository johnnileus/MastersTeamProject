#pragma once
#include "AdvanceMeshMaterial.h"
#include "OGLRenderer.h"

namespace NCL {
	using namespace Rendering;

	class MeshGeometry;
	namespace CSC8503 {
		class ToonTransform;
		class ToonGameObject;
		using namespace Maths;

		class AdvanceRenderObject
		{
		public:
			AdvanceRenderObject(ToonTransform* parentTransform, MeshGeometry* mesh, Texture* tex, Shader* shader, MeshGeometry* minimapMesh = nullptr);
			AdvanceRenderObject(ToonTransform* parentTransform, MeshGeometry* mesh, AdvanceMeshMaterial* advMat, Shader* shader, MeshGeometry* minimapMesh = nullptr);
			AdvanceRenderObject(MeshGeometry* mesh, Texture* tex, Shader* shader);
			~AdvanceRenderObject();

			void SetDefaultTexture(Texture* t) {
				texture = t;
			}

			Texture* GetDefaultTexture() const {
				return texture;
			}

			MeshGeometry*	GetMesh() const {
				return mesh;
			}
			MeshGeometry* GetMinimapMesh() const {
				return minimapMesh;
			}

			void SetMesh(MeshGeometry* mesh)		{ this->mesh = mesh; }
			void SetMinimapMesh(MeshGeometry* mesh) { this->minimapMesh = mesh; }

			ToonTransform*		GetTransform() const {
				return transform;
			}

			Shader*		GetShader() const {
				return shader;
			}

			void SetShader(Shader* newShader) {
				shader = newShader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}

			void SetMinimapColour(const Vector4& c) { minimapColour = c; }
			Vector4 GetMinimapColour() const { return minimapColour; }

			void SetGameObject(ToonGameObject* gameObject) { this->gameObject = gameObject; }

			ToonGameObject* GetGameObject() const {
				return gameObject;
			}

			AdvanceMeshMaterial* GetMaterial() const { return material; }
			void SetMeshMaterial(AdvanceMeshMaterial* newMat) { material = newMat; }

		protected:
			ToonGameObject* gameObject;
			MeshGeometry*	mesh;
			MeshGeometry*	minimapMesh;
			Texture*	texture;
			Shader*		shader;
			ToonTransform*	transform;
			Vector4			colour;
			Vector4			minimapColour;

			AdvanceMeshMaterial*	material;
			std::vector<const Texture*> mTexturesDiffuse;
			std::vector<const Texture*> mTexturesBump;
		};
	}
}

