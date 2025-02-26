#pragma once
#include <vector>
#include "Vector.h"

namespace NCL {
	namespace CSC8503 {
		class NavMeshNode {
		public:
			NavMeshNode() {};
			~NavMeshNode() {};
			int getCost() {
				return this->cost;
			}
			NCL::Maths::Vector3 GetPosition() {
				return this->position;
			}
		protected:
			bool checkPassibility();
			int calculateCost();
			void setCost(int cost) {
				this->cost = cost;
			}
			int cost;
			NCL::Maths::Vector3 position;
		};
	}
}