#include "RenderObject.h"
#include "Mesh.h"

using namespace NCL::CSC8503;
using namespace NCL;

void RenderObject::SetAnimation(MeshAnimation& inAnim) {
	anim = &inAnim;

	skeleton.resize(anim->GetJointCount());
}

void RenderObject::UpdateAnimation(float dt) {
	if (!mesh || !anim) {
		return;
	}
	animTime -= dt;

	if (animTime <= 0) {
		currentAnimFrame++;
		animTime += anim->GetFrameTime();
		currentAnimFrame = (currentAnimFrame++) % anim->GetFrameCount();

		std::vector<Matrix4>const& inverseBindPose = mesh->GetInverseBindPose();

		if (inverseBindPose.size() != anim->GetJointCount()) {
			//oh no
			return;
		}

		const Matrix4* joints = anim->GetJointData(currentAnimFrame);

		for (int i = 0; i < skeleton.size(); ++i) {
			skeleton[i] = joints[i] * inverseBindPose[i];
		}
	}
}

//RenderObject::RenderObject(Transform* parentTransform, Mesh* mesh, Texture* tex, Shader* shader) { //this isnt in the ps5 starter stuff idk why (edit: its because we do this in the header instead)
//	if (!tex) {
//		bool a = true;
//	}
//	this->transform	= parentTransform;
//	this->mesh		= mesh;
//	this->texture	= tex;
//	this->shader	= shader;
//	this->colour	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
//}

RenderObject::~RenderObject() {

}