#pragma once
#include "Vector.h"
#include "GameObject.h"
#include "SphereVolume.h"

namespace NCL {
	namespace CSC8503 {
		class DemoShootableEnemy {
		public:
			DemoShootableEnemy(float scale, float inverseMass, NCL::Maths::Vector3 spawnPostion, float health = 100, bool alive = true, float respawnTimer = 15.0f) {
				this->scale = scale;
				this->inverseMass = inverseMass;
				this->spawnPosition = spawnPostion;
				this->enemy = InitialiseEnemy(this->scale, this->inverseMass, this->spawnPosition);
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
			GameObject* InitialiseEnemy(float scale, float inverseMass, NCL::Maths::Vector3 postion);
			GameObject* GetGameObject() { return this->enemy; }
			void UpdateRespawnTimer(float dt) { this->timeToRespawn -= dt; }
			bool CheckRespawn() { return respawnTimer <= 0; }
			void Spawn();
			void KillEnemy();
			void UpdateHealth(float damage) { this->currentHealth -= damage; }
			void RegisterHit();

			float GetCurrentHealth() { return this->currentHealth; }
			NCL::Maths::Vector3 GetCurrentPosition() { return this->enemy->GetTransform().GetPosition(); }
		protected:
			float maxHealth;
			float currentHealth;
			GameObject* enemy;
			NCL::Maths::Vector3 spawnPosition;
			float scale;
			float inverseMass;
			float respawnTimer;
			float timeToRespawn;
			bool alive;
		};
	}
}