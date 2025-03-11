#include "DemoShootableEnemy.h"

using namespace NCL;
using namespace CSC8503;

GameObject* DemoShootableEnemy::InitialiseEnemy(float scale, float inverseMass, NCL::Maths::Vector3 position) {
	GameObject* e = new GameObject();
	SphereVolume* volume = new SphereVolume(1.0f);

	e->SetBoundingVolume((CollisionVolume*)volume);

	e->GetTransform()
		.SetScale(Vector3(scale, scale, scale))
		.SetPosition(position);
}