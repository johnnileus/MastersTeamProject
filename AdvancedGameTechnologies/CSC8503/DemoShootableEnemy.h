#pragma once
#include "Vector.h"
#include "GameObject.h"
#include "SphereVolume.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class DemoShootableEnemy : GameObject {
		public:
			DemoShootableEnemy(GameWorld* world, float scale, float inverseMass, NCL::Maths::Vector3 spawnPostion, float health = 100, bool alive = true, float respawnTimer = 15.0f) {
				this->scale = scale;
				this->inverseMass = inverseMass;
				this->spawnPosition = spawnPostion;
				InitialiseEnemy(world, this->scale, this->inverseMass, this->spawnPosition);
				this->respawnTimer = respawnTimer;
				this->timeToRespawn = 0.0f;
				this->maxHealth = health;
				Spawn();
			}
			~DemoShootableEnemy() {
				this->currentHealth = NULL;
				this->respawnTimer = NULL;
				this->alive = NULL;
			}
			void InitialiseEnemy(GameWorld* world, float scale, float inverseMass, NCL::Maths::Vector3 postion);
			void UpdateRespawnTimer(float dt) { this->timeToRespawn -= dt; }
			bool CheckRespawn() { return respawnTimer <= 0; }
			void Spawn();
			void KillEnemy();
			void UpdateHealth(float damage) { this->currentHealth -= damage; }
			void RegisterHit();

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
		};
	}
}