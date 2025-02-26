#pragma once
#include "NavMeshNode.h"
#include <vector>

class NavMeshNode;

namespace CSC8503 {
	class NavMeshGrid {
	public:
		std::vector<NavMeshNode>findRoute(NavMeshNode startNode, NavMeshNode endNode);
	protected:
		int levelSize;
		std::vector<std::vector<NavMeshNode>>generateNavMeshGrid(int levelSize);

		//detect placed objects such as rocks and trees while ignoring objects such as players, 
		//calculate the bounds of these objects and all nodes they would effect
		//nodePosition +- 0.5 should be the area around each node
		void detectObstructedNodes();

	};
}