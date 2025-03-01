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
			}
			void checkObstructed();
			bool isObstructed() {
				return obstructed;
			}
			bool detectImpassableEdge();
			void calculateEdges();
			void addEdge(NavMeshNode* neighbour, int cost) {
				Edge e;
				e.neighbour = neighbour;
				e.cost = cost;
				edges.push_back(e);
			}
		protected:
			std::vector<Edge> edges;
			NCL::Maths::Vector3 position;
			bool obstructed;
		};
	}
}