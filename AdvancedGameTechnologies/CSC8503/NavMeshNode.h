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
				this->nodeInUse = false;
			}
			void checkObstructed();
			bool isObstructed() {
				return obstructed;
			}
			bool detectImpassableEdge(NavMeshNode* neighbour);
			void calculateEdges();
			void addEdge(NavMeshNode* neighbour, int cost);
			float getGScore(){ return this->gScore; }
			float getFScore(){ return this->fScore; }
			void setGScore(float gScore) { this->gScore = gScore; }
			void setFScore(float fScore) { this->gScore = fScore; }

			bool IsNodeInUse() { return false; }

			void SetNodeInUse() { this->nodeInUse = true; }
			void SetNodeNotInUse() { this->nodeInUse = false; }
		protected:
			std::vector<Edge> edges;
			NCL::Maths::Vector3 position;
			bool obstructed;

			float gScore;
			float fScore;
			bool nodeInUse;
		};
	}
}