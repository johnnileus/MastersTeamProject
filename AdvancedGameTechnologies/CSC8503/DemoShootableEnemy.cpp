#include "DemoShootableEnemy.h"
#include "TutorialGame.h"

using namespace NCL;
using namespace CSC8503;

GameObject* DemoShootableEnemy::InitialiseEnemy(float scale, float inverseMass, NCL::Maths::Vector3 position) {
	GameObject* e = new GameObject();
	SphereVolume* volume = new SphereVolume(1.0f);

	e->SetBoundingVolume((CollisionVolume*)volume);

	e->GetTransform()
		.SetScale(Vector3(scale, scale, scale))
		.SetPosition(position);

	e->SetRenderObject(new RenderObject(&e->GetTransform(), AssetManager::Instance().sphereMesh, AssetManager::Instance().metalTex, AssetManager::Instance().basicShader));
	e->SetPhysicsObject(new PhysicsObject(&e->GetTransform(), e->GetBoundingVolume()));
	e->GetPhysicsObject()->SetInverseMass(inverseMass);
	e->GetPhysicsObject()->InitSphereInertia();

	return e;
}
}