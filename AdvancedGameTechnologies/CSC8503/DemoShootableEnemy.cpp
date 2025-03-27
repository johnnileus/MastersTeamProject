#include "DemoShootableEnemy.h"
#include "SceneManager.h"
using namespace NCL;
using namespace CSC8503;

void DemoShootableEnemy::InitialiseEnemy(float scale, float inverseMass, NCL::Maths::Vector3 position) {
	
}

void DemoShootableEnemy::KillEnemy() {
	this->GetPhysicsObject();//need a function to make physics objects inactive
	this->alive = false;
	this->GetTransform().SetPosition(Vector3(0, -50, 0));
	this->timeToRespawn = this->respawnTimer;
}

//function allows us to add additional logic to hits, e.g critical hit, change in behaviour etc
void DemoShootableEnemy::RegisterHit() {
	UpdateHealth(15.0f);
	std::cout << this->currentHealth;
	if (this->currentHealth <= 0) {
		KillEnemy();
		std::cout << "Enemy Killed";
	}
}

//this function is not being called for some reason
void DemoShootableEnemy::OnCollisionBegin(GameObject* otherObject) {
	std::cout << "collision" << std::endl;
	if (otherObject->tag == "Bullet") { RegisterHit(); }
}