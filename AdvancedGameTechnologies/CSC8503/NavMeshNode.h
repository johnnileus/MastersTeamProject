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
			std::vector<Edge> GetEdges() { return this->edges; }
			NCL::Maths::Vector3 GetPosition() {	return this->position; }
			float getGScore(){ return this->gScore; }
			float getFScore(){ return this->fScore; }
			float getHeuristic(){ return this->heuristic; }
			void setGScore(float gScore) { this->gScore = gScore; }
			void setFScore(float fScore) { this->gScore = fScore; }
			void setHeuristic(float heuristic) { this->heuristic = heuristic; }

			bool IsNodeInUse() { return false; }
		protected:
			std::vector<Edge> edges;
			NCL::Maths::Vector3 position;
			bool obstructed;
			float gScore;
			float fScore;
			float heuristic;
		};
	}
}