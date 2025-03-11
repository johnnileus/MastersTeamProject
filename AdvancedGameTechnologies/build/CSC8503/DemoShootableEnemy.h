#pragma once
#include "Vector.h"
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class DemoShootableEnemy {
		public:
			DemoShootableEnemy(NCL::Maths::Vector3 position, float initialHealth) {
				this->respawnTimer = 15.0f;
				Spawn(position, initialHealth);
			}
			~DemoShootableEnemy() {
				this->health = NULL;
				this->respawnTimer = NULL;
				this->alive = NULL;
			}
			void UpdateRespawnTimer(float dt) { respawnTimer -= dt; }
			bool CheckRespawn() { return respawnTimer <= 0; }
			void Spawn(NCL::Maths::Vector3 position, float initialHealth);
			void KillEnemy();
			void UpdateHealth(float damage) { this->health -= damage; }
			void RegisterHit();

			float GetCurrentHealth() { return this->health; }
			NCL::Maths::Vector3 GetCurrentPosition();
		protected:
			float health;
			GameObject* enemy;
			float respawnTimer;
			bool alive;
		};
	}
}