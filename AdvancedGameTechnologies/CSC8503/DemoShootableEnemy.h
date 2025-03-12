#pragma once
#include "Vector.h"
#include "GameObject.h"
#include "SphereVolume.h"

namespace NCL {
	namespace CSC8503 {
		class DemoShootableEnemy {
		public:
			DemoShootableEnemy() {
				this->enemy = InitialiseEnemy();
				this->respawnTimer = 15.0f;
				Spawn();
			}
			~DemoShootableEnemy() {
				this->currentHealth = NULL;
				this->respawnTimer = NULL;
				this->alive = NULL;
			}
			GameObject* InitialiseEnemy(float scale, float inverseMass, NCL::Maths::Vector3 postion);
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