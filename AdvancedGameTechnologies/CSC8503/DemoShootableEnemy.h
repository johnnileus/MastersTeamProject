#pragma once
#include "Vector.h"
#include "GameObject.h"
#include "SphereVolume.h"
#include "assetManager.h"
#include "RenderObject.h"
#include "PhysicsObject.h"

namespace NCL {
	namespace CSC8503 {
		class DemoShootableEnemy : public GameObject {
		public:
			DemoShootableEnemy(float scale = 1.0f, float inverseMass = 100.0f, NCL::Maths::Vector3 spawnPostion = NCL::Maths::Vector3(0,0,0), float health = 100, bool alive = true, float respawnTimer = 15.0f, float damage = 10.0f) {
				InitialiseDemoShootableEnemy();
			}
			~DemoShootableEnemy() {
				this->currentHealth = NULL;
				this->respawnTimer = NULL;
				this->alive = NULL;
			}

			void InitialiseDemoShootableEnemy(float scale = 1.0f, float inverseMass = 100.0f, 
				NCL::Maths::Vector3 spawnPostion = NCL::Maths::Vector3(0, 0, 0), float health = 100, 
				bool alive = true, float respawnTimer = 15.0f, float damage = 10.0f) {

				this->scale = scale;
				this->inverseMass = inverseMass;
				this->spawnPosition = spawnPostion;

				SphereVolume* volume = new SphereVolume(1.0f);

				this->SetBoundingVolume((CollisionVolume*)volume);

				this->GetTransform()
					.SetScale(Vector3(scale, scale, scale))
					.SetPosition(spawnPosition);

				this->SetRenderObject(new RenderObject(&this->GetTransform(), AssetManager::Instance().sphereMesh, AssetManager::Instance().metalTex, AssetManager::Instance().basicShader));
				this->SetPhysicsObject(new PhysicsObject(&this->GetTransform(), this->GetBoundingVolume()));
				this->GetPhysicsObject()->SetInverseMass(inverseMass);
				this->GetPhysicsObject()->InitSphereInertia();

				this->tag = "Enemy";
				this->respawnTimer = respawnTimer;
				this->timeToRespawn = 0.0f;
				this->maxHealth = health;
				this->damage = damage;
			}
			void InitialiseEnemy(float scale, float inverseMass, NCL::Maths::Vector3 postion);
			void UpdateRespawnTimer(float dt) { this->timeToRespawn -= dt; }
			bool CheckRespawn() { return this->timeToRespawn <= 0; }
			void Spawn();
			void KillEnemy();
			void UpdateHealth(float damage) { this->currentHealth -= damage; }
			void RegisterHit();
			bool CheckAlive() { return this->alive; }
			void OnCollisionBegin(GameObject* otherObject) override;

			float GetCurrentHealth() { return this->currentHealth; }
			NCL::Maths::Vector3 GetCurrentPosition() { return this->GetTransform().GetPosition(); }
		protected:
			float maxHealth;
			float currentHealth;
			NCL::Maths::Vector3 spawnPosition;
			float scale;
			float inverseMass;
			float respawnTimer;
			float timeToRespawn;
			bool alive;
			float damage;
		};
	}
}