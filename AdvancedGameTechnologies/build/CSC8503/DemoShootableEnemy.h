#pragma once
#include "Vector.h"

namespace NCL {
	namespace CSC8503 {
		class DemoShootableEnemy {
		public:
			DemoShootableEnemy();
			~DemoShootableEnemy();
			void UpdateRespawnTimer(float dt) { respawnTimer -= dt; }
			bool CheckRespawn();
			void Spawn();
			void KillEnemy();
			void UpdateHealth();
			void RegisterHit();
		protected:
			float health;
			NCL::Maths::Vector3 position;
			float respawnTimer;
			bool alive;
		};
	}
}