#include "DemoShootableEnemy.h"
#include "SceneManager.h"
using namespace NCL;
using namespace CSC8503;

void DemoShootableEnemy::InitialiseEnemy(GameWorld* world, float scale, float inverseMass, NCL::Maths::Vector3 position) {
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
	e->tag = "Enemy";
	world->AddGameObject(e);
}

void DemoShootableEnemy::Spawn() {
	this->currentHealth = this->maxHealth;
	this->alive = true;
	this->GetTransform().SetPosition(this->spawnPosition);
	this->GetPhysicsObject();//need a function to make physics objects active
	std::cout << "Enemy Spawned";
}

void DemoShootableEnemy::KillEnemy() {
	this->GetPhysicsObject();//need a function to make physics objects inactive
	this->alive = false;
	this->GetTransform().SetPosition(Vector3(0, -50, 0));
	this->timeToRespawn = this->respawnTimer;
}

//function allows us to add additional logic to hits, e.g critical hit, change in behaviour etc
void DemoShootableEnemy::RegisterHit() {
	UpdateHealth(100.0f);
	std::cout << this->currentHealth;
	if (this->currentHealth <= 0) {
		KillEnemy();
		std::cout << "Enemy Killed";
	}
}

void DemoShootableEnemy::OnCollisionBegin(GameObject* otherObject) {
	std::cout << "collision" << std::endl;
	if (otherObject->tag == "Bullet") { RegisterHit(); }
}