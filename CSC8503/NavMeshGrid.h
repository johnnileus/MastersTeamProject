#pragma once
#include "NavMeshNode.h"
#include <vector>
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class NavMeshNode;
		class NavMeshGrid {
		public:
			NavMeshGrid(GameWorld* world) { 
				levelSize = 256; 
				this->world = world;
				nodeGrid = GenerateNavMeshGrid(levelSize);
				for (int x = 0; x < levelSize; ++x) {
					for (int z = 0; z < levelSize; ++z) {
						if (!nodeGrid[x][z]->isObstructed()) {
							allNodes.emplace_back(nodeGrid[x][z]);
						}
						
					}
				}
			}
			~NavMeshGrid() {}
			std::vector<NavMeshNode*>GetAllNodes() { return this->allNodes; }
			int GetLevelSize() { return this->levelSize; }
		protected:
			int levelSize;
			std::vector<std::vector<NavMeshNode*>>nodeGrid;
			std::vector<std::vector<NavMeshNode*>>GenerateNavMeshGrid(int levelSize);
			std::vector<NavMeshNode*>allNodes;
			GameWorld* world;
		};
	}
}