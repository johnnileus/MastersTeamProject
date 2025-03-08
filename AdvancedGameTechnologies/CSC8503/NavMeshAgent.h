#pragma once
#include "NavMeshGrid.h"
#include "NavMeshNode.h"
#include <vector>

namespace NCL {
	namespace CSC8503 {
		class NavMeshGrid;
		class NavMeshNode;
		class NavMeshAgent {//logic to follow the path generated by this class to be determined by each enemy type
			
		public:
			NavMeshAgent() {
				this->newFScore = 0;
				this->newGScore = 0;
				this->newHeuristic = 0;
			}
			void SetLevelGrid(NavMeshGrid* levelGrid) { this->currentLevelGrid = levelGrid; }
			void FindPath(NavMeshNode* currentNode, NavMeshNode* destination);
			void clearPath();//clear path, open list and closed list
			float calculateHeuristic(NavMeshNode* node, NavMeshNode* destination);
			float calculateGScore(NavMeshNode* node);
			float calculateFScore(NavMeshNode* node);
			void setCurrentNode();
		protected:
			float newFScore;
			float newGScore;
			float newHeuristic;
			std::vector<NavMeshNode*> openList;
			std::vector<NavMeshNode*> closedList;
			std::vector<NavMeshNode*> path;
			NavMeshGrid* currentLevelGrid;
			NavMeshNode* destination;
			NavMeshNode* currentNode;
			NavMeshNode* node; //maybe should be local variable
		};
	}
}