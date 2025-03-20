#pragma once
#include "NavMeshGrid.h"
#include "NavMeshNode.h"
#include <vector>
#include "DemoShootableEnemy.h"

namespace NCL {
	namespace CSC8503 {
		class NavMeshGrid;
		class NavMeshNode;
		class NavMeshAgent : public DemoShootableEnemy {
		public:
			NavMeshAgent(NavMeshGrid* nodeGrid, float scale, float inverseMass, NCL::Maths::Vector3 spawnPostion, float health = 100, bool alive = true, float respawnTimer = 15.0f) {
				DemoShootableEnemy(scale, inverseMass, spawnPostion, health, alive, respawnTimer);
				this->nodeGrid = nodeGrid;
				this->newFScore = 0;
				this->newGScore = 0;
				this->newHeuristic = 0;
				this->destination = nullptr;
				this->currentNode = nullptr;
				this->nextNode = nullptr;
			}
			void FindPath();
			void clearPath();//clear path, open list and closed list
			float calculateHeuristic(NavMeshNode* node, NavMeshNode* destination);
			float calculateGScore(NavMeshNode* node, float edgeCost);
			float calculateFScore(float heuristic, float gScore);
			void setCurrentNode();
			void FollowPath();
			void MoveTowardsNextNode();
			void SetDestination();
		protected:
			float newFScore;
			float newGScore;
			float newHeuristic;
			std::vector<NavMeshNode*> openList;
			std::vector<NavMeshNode*> closedList;
			std::vector<NavMeshNode*> path;
			NavMeshGrid* nodeGrid;
			NavMeshNode* destination;
			NavMeshNode* currentNode;
			NavMeshNode* nextNode;
		};
	}
}