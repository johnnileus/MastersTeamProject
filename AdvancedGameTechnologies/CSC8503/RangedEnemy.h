#pragma once
#include "NavMeshAgent.h"
#include "StateMachine.h"
#include "Player.h"

namespace NCL {
	namespace CSC8503 {
		class RangedEnemy : StateMachine, NavMeshAgent {
		public:
			RangedEnemy();
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