#pragma once
#include "NavMeshNode.h"
#include <vector>

namespace NCL {
	namespace CSC8503 {
		class NavMeshNode;
		class NavMeshGrid {
		public:
			NavMeshGrid() { 
				levelSize = 256; 
				nodeGrid = GenerateNavMeshGrid(levelSize);
				for (int x = 0; x < levelSize; ++x) {
					for (int z = 0; z < levelSize; ++z) {
						allNodes.emplace_back(nodeGrid[x][z]);
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
		};
	}
}