#pragma once
#include "NavMeshNode.h"
#include <vector>

namespace NCL {
	namespace CSC8503 {
		class NavMeshGrid {
		public:
			NavMeshGrid() { levelSize = 256; nodeGrid = GenerateNavMeshGrid(levelSize); }
			~NavMeshGrid() {}
			std::vector<NavMeshNode>findRoute(NavMeshNode startNode, NavMeshNode endNode);
		protected:
			int levelSize;
			std::vector<std::vector<NavMeshNode>>nodeGrid;
			std::vector<std::vector<NavMeshNode>>GenerateNavMeshGrid(int levelSize);


		};
	}
}