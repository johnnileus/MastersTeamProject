#pragma once
#include <vector>

namespace NCL {
	namespace CSC8503 {
		class NavMeshNode;

		struct Edge {
			NavMeshNode* neighbour;
			int cost;
		};

		class NavMeshNode {
		public:
			NavMeshNode(NCL::Maths::Vector3 position) {
				this->position = position;
				this->obstructed = false;
				this->heuristic = 0;
				this->gScore = 0;
				this->fScore = 0;
				this->nodeInUse = false;
			}
			void checkObstructed();
			bool isObstructed() {
				return obstructed;
			}
			bool detectImpassableEdge(NavMeshNode* neighbour);
			void calculateEdges();
			void addEdge(NavMeshNode* neighbour, int cost);
			float calculateHeuristic(NavMeshNode* neighbour);
			float calculateGScore(float previousGScore, float edgeCost) { return previousGScore + edgeCost; }
			float calculateFScore() { return this->heuristic + this->gScore; }

			bool IsNodeInUse() { return false; }

			void SetNodeInUse() { this->nodeInUse = true; }
			void SetNodeNotInUse() { this->nodeInUse = false; }
		protected:
			std::vector<Edge> edges;
			NCL::Maths::Vector3 position;
			bool obstructed;

			float heuristic;
			float gScore;
			float fScore;
			bool nodeInUse;
		};
	}
}