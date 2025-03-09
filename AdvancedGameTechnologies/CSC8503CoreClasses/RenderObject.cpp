#include "RenderObject.h"
#include "Mesh.h"
using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(Transform* parentTransform, Mesh* mesh, Texture* tex, Shader* shader)
	:RenderObject(parentTransform,mesh,tex,shader,Enums::RenderObjectType::Default){}

RenderObject::RenderObject(Transform* parentTransform, Mesh* mesh, Texture* tex, Shader* shader,Enums::RenderObjectType type) {
	if (!tex) {
		bool a = true;
	}
	this->transform	= parentTransform;
	this->mesh		= mesh;
	this->texture	= tex;
	this->shader	= shader;
	this->colour	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	this->renderType = type;
}

RenderObject::~RenderObject() {

}