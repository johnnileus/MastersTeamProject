#pragma once
#include "Vector.h"
#include "GameObject.h"

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
				this->health = NULL;
				this->respawnTimer = NULL;
				this->alive = NULL;
			}
			GameObject* InitialiseEnemy(float scale, float inverseMass);
			void UpdateRespawnTimer(float dt) { respawnTimer -= dt; }
			bool CheckRespawn() { return respawnTimer <= 0; }
			void Spawn();
			void KillEnemy();
			void UpdateHealth(float damage) { this->health -= damage; }
			void RegisterHit();

			float GetCurrentHealth() { return this->health; }
			NCL::Maths::Vector3 GetCurrentPosition() { return this->enemy->GetTransform().GetPosition(); }
		protected:
			float health;
			GameObject* enemy;
			float respawnTimer;
			bool alive;
		};
	}
}