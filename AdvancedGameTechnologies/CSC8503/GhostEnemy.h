#pragma once
#include "NavMeshAgent.h"
#include "StateMachine.h"
#include "Player.h"

namespace NCL {
	namespace CSC8503 {
		class GhostEnemy : NavMeshAgent {
		public:
			GhostEnemy();
			void InitStateMachine();
			void PatrolState();
			void ChaseState();
			void AttackState();
			void RetreatState();
			void GetPlayerNode();
			void UpdateEnemy();
		protected:
			StateMachine* stateMachine;
			Player* currentTarget;
		};
	}
}
