#pragma once
#include "NavMeshGrid.h"
#include "NavMeshNode.h"
#include <vector>
#include "DemoShootableEnemy.h"
#include "unordered_set"

namespace NCL {
	namespace CSC8503 {
		class NavMeshGrid;
		class NavMeshNode;
		class NavMeshAgent : public DemoShootableEnemy {
		public:
			NavMeshAgent(NavMeshGrid* nodeGrid = nullptr, float scale = 1.0f, float inverseMass = 1.0f, NCL::Maths::Vector3 spawnPostion = NCL::Maths::Vector3(0,0,0), float health = 100, bool alive = true, float respawnTimer = 15.0f, float damage = 10.0f) {
				this->InitialiseDemoShootableEnemy(scale, inverseMass, spawnPosition, health, alive, respawnTimer, damage);
				this->InitialiseNavMeshAgent(nodeGrid);
			}
			void InitialiseNavMeshAgent(NavMeshGrid* nodeGrid = nullptr) {
				
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
			void setCurrentNode(int x, int y);
			void FollowPath();
			void MoveTowardsNextNode();
			void SetDestination();
			void SetDestinationNull();
			void Spawn();
		protected:
			float newFScore;
			float newGScore;
			float newHeuristic;
			std::unordered_set<NavMeshNode*> openList;
			std::unordered_set<NavMeshNode*> closedList;
			std::vector<NavMeshNode*> path;
			NavMeshGrid* nodeGrid;
			NavMeshNode* destination;
			NavMeshNode* currentNode;
			NavMeshNode* nextNode;
		};
	}
}