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
			NavMeshAgent() {
				this->newFScore = 0;
				this->newGScore = 0;
				this->heuristic = 0;
				this->currentLevelGrid = NULL;
				this->currentNode = NULL;
				this->node = NULL;
				this->destination = NULL;
			}
			std::vector<NavMeshNode> FindPath(NavMeshNode* currentNode, NavMeshNode* destination);
			void clearPath();
			float calculateHeuristic(NavMeshNode* currentNode, NavMeshNode* destination);
			float calculateGScore(NavMeshNode* node);
			float calculateFScore(NavMeshNode* node);
			void setCurrentNode();
		protected:
			float newFScore;
			float newGScore;
			float heuristic;
			std::vector<NavMeshNode*> openList;
			std::vector<NavMeshNode*> closedList;
			std::vector<NavMeshNode*> path;
			NavMeshGrid* currentLevelGrid;
			NavMeshNode* destination;
			NavMeshNode* currentNode;
			NavMeshNode* node;
		};
	}
}