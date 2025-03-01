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
				position = position;
				obstructed = false;
			}
			void checkObstructed();
			bool isObstructed() {
				return obstructed;
			}
			bool detectImpassableEdge(NavMeshNode* neighbour);
			void calculateEdges();
			void addEdge(NavMeshNode* neighbour, int cost);
		protected:
			std::vector<Edge> edges;
			NCL::Maths::Vector3 position;
			bool obstructed;
		};
	}
}