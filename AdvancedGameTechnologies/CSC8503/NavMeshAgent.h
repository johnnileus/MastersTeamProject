#pragma once
#include "NavMeshGrid.h"
#include "NavMeshNode.h"
#include <vector>

class NavMeshGrid;
class NavMeshNode;

namespace NCL {
	namespace CSC8503 {
		class NavMeshAgent {
		public:
			std::vector<NavMeshNode> FindPath(NavMeshNode currentNode, NavMeshNode destination);
			void clearPath();
		protected:
			float newFScore;
			float newGScore;
			float newHeuristic;
			std::vector<NavMeshNode*> openList;
			std::vector<NavMeshNode*> closedList;
			std::vector<NavMeshNode*> path;
			NavMeshNode* destination;
			NavMeshNode* currentNode;
		};
	}
}