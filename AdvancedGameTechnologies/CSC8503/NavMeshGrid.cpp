#include "NavMeshGrid.h"
#include "NavMeshNode.h"
#include <vector>

using namespace NCL;
using namespace CSC8503;

std::vector<std::vector<NavMeshNode>> NavMeshGrid::GenerateNavMeshGrid(int levelSize) {
	std::vector<std::vector<NavMeshNode>>nodeGrid(levelSize, std::vector<NavMeshNode>(levelSize));
	for (int x = 0; x < levelSize; ++x) {
		for (int z = 0; z < levelSize; ++z) {
			NCL::Maths::Vector3 nodePosition = NCL::Maths::Vector3(x - levelSize / 2, 500, z - levelSize / 2);
			nodeGrid[x][z] = NavMeshNode(nodePosition);
		}
	}
	for (int x = 0; x < std::size(nodeGrid); ++x) {
		for (int z = 0; z < std::size(nodeGrid); ++z) {
			//check if the neighbour exists
			//9 cases, 4 corners, 4 edges, somewhere in the middle
			if (x - 1 < 0 && z - 1 < 0) {
				
			}
			//calculate the cost of travel, currently 1 or 0
			//update the nodes edges vector
		}
	}
}