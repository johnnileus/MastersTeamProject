#pragma once
#include "NavMeshAgent.h"
#include "StateMachine.h"
#include "Player.h"
#include <vector>
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class MeleeEnemy : public NavMeshAgent {
		public:
			MeleeEnemy(NavMeshGrid* nodeGrid = nullptr, float scale = 1.0f, float inverseMass = 1.0f, NCL::Maths::Vector3 spawnPostion = NCL::Maths::Vector3(0,0,0), float health = 100, bool alive = true, float respawnTimer = 15.0f, float damage = 10.0f, GameWorld* world = nullptr) {
				this->InitialiseDemoShootableEnemy(scale, inverseMass, spawnPosition, health, alive, respawnTimer, damage);
				this->InitialiseNavMeshAgent(nodeGrid);
				InitialiseMeleeEnemy(world);
			}
			void InitStateMachine();
			void PatrolState();
			void ChaseState();
			void AttackState(float dt);
			void RetreatState();
			void RestState(float dt);
			void UpdateEnemy(float dt);
			void InitialiseMeleeEnemy(GameWorld* world) {

				this->stateMachine = new StateMachine();
				this->currentTarget = nullptr;
				this->attackCooldown = 3;
				this->chargeForce = 5;

				auto findPlayers = [this](GameObject* obj) {
					if (obj->tag == "Player") {
#ifdef USEAGC
						Player* player = static_cast<Player*>(obj);
#else
						Player* player = dynamic_cast<Player*>(obj);
#endif // USEAGC
						if (player) {
							this->players.push_back(player);
						}
					}
					};

				world->OperateOnContents(findPlayers);

				this->InitStateMachine();
				this->GetRenderObject()->SetColour(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
			}
			void OnCollisionBegin(GameObject* otherObject) override;
		protected:
			StateMachine* stateMachine;
			Player* currentTarget;
			float attackCooldown;
			float chargeForce;
			std::vector<Player*> players;
			float hitCooldown;
		};
	}
}