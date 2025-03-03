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
			}
			void checkObstructed();
			bool isObstructed() {
				return obstructed;
			}
			bool detectImpassableEdge(NavMeshNode* neighbour);
			void calculateEdges();
			void addEdge(NavMeshNode* neighbour, int cost);
			float calculateHeuristic(NavMeshNode* neighbour);
			float calculateGScore(NavMeshNode* previousNeighbour);
			float calculateFScore() { return this->heuristic + this->gScore; }
		protected:
			std::vector<Edge> edges;
			NCL::Maths::Vector3 position;
			bool obstructed;

			float heuristic;
			float gScore;
			float fScore;
		};
	}
}