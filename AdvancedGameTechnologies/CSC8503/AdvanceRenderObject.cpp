#include "AdvanceRenderObject.h"

using namespace NCL::CSC8503;
using namespace NCL;



AdvanceRenderObject::AdvanceRenderObject(ToonTransform* parentTransform, MeshGeometry* mesh, Texture* tex, Shader* shader, MeshGeometry* minimapMesh) {
    this->transform	= parentTransform;
    this->mesh		= mesh;
    this->texture	= tex;
    this->material = nullptr;
    this->shader	= shader;
    this->colour	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    this->minimapColour	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    this->minimapMesh = minimapMesh;
}

AdvanceRenderObject::AdvanceRenderObject(ToonTransform* parentTransform, MeshGeometry* mesh, AdvanceMeshMaterial* advMat, Shader* shader, MeshGeometry* minimapMesh)
{
    this->transform = parentTransform;
    this->mesh = mesh;
    this->texture = nullptr;
    this->shader = shader;
    this->colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    this->minimapColour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    this->minimapMesh = minimapMesh;

    this->material = advMat;
    mTexturesDiffuse = this->material->GetDiffuseTextures();
    mTexturesBump = this->material->GetBumpTextures();
}

AdvanceRenderObject::AdvanceRenderObject(MeshGeometry* mesh, Texture* tex, Shader* shader)
{
    this->transform = nullptr;
    this->mesh = mesh;
    this->texture = tex;
    this->shader = shader;
    this->colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

AdvanceRenderObject::~AdvanceRenderObject()
{
    
}