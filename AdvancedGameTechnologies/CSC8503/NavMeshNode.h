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
			
		protected:
			std::vector<Edge> edges;
			NCL::Maths::Vector3 position;
			bool obstructed;
			bool checkObstructed();
			bool detectImpassableEdge();
		};
	}
}