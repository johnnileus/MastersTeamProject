#pragma once
#include <reactphysics3d/engine/Material.h>

#include "Texture.h"
#include "Mesh.h"
#include "MeshMaterial.h"

#include "MeshAnimation.h"

#include "Buffer.h"
#include <Shader.h>

namespace NCL {
	using namespace NCL::Rendering;
	namespace CSC8503 {
		class Transform;
		using namespace Maths;

		class RenderObject
		{
		public:
			RenderObject(Transform* parentTransform, Mesh* inMesh, Texture* tex, Shader* inShader) {
				buffer = nullptr;
				anim = nullptr;

				transform = parentTransform;
				mesh = inMesh;
				texture = tex;
				shader = inShader;
				colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			}
			~RenderObject();

			void SetDefaultTexture(Texture* t) {
				texture = t;
			}

			Texture* GetDefaultTexture() const {
				return texture;
			}

			Buffer* GetGPUBuffer() const { //if there is a problem with this, make it #ifdef USEAGC
				return buffer;
			}

			void SetGPUBuffer(Buffer* b) {
				buffer = b;
			}

			Mesh*	GetMesh() const {
				return mesh;
			}

			Transform*		GetTransform() const {
				return transform;
			}

			Shader*		GetShader() const {
				return shader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}
			MeshMaterial* GetMaterial()const
			{
				return material;
			}
			void SetMaterial(MeshMaterial* material)
			{
				this->material = material;
			}

			void SetAnimation(MeshAnimation& inAnim);

			void UpdateAnimation(float dt);

			std::vector<Matrix4>& GetSkeleton() {
				return skeleton;
			}

		protected:
			Buffer* buffer;
			Mesh*		mesh;
			Texture*	texture;
			Shader*		shader;
			Transform*	transform;
			Vector4		colour;
			MeshMaterial* material;

			MeshAnimation* anim;

			std::vector<Matrix4> skeleton;
			float	animTime = 0.0f;
			int currentAnimFrame = 0;
		};
	}
}
