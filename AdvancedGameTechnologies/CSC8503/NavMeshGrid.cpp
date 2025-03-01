#include "NavMeshGrid.h"
#include "NavMeshNode.h"
#include <vector>

using namespace NCL;
using namespace CSC8503;

//generate navmeshgrid centred at 0,0,0
std::vector<std::vector<NavMeshNode>> NavMeshGrid::GenerateNavMeshGrid(int levelSize) {
	std::vector<std::vector<NavMeshNode>> nodeGrid;
	for (int x = 0; x < levelSize; ++x) {
		for (int z = 0; z < levelSize; ++z) {
			NCL::Maths::Vector3 nodePosition = NCL::Maths::Vector3(x - levelSize/2, 500, z - levelSize/2);//replace 500 with the height of the vertex for the node
			nodeGrid[x][z] = NavMeshNode(nodePosition);
			nodeGrid[x][z].checkObstructed();
		}
	}
	for (int x = 0; x < std::size(nodeGrid); ++x) {
		for (int z = 0; z < std::size(nodeGrid); ++z) {
			if (x - 1 < 0 && z - 1 < 0) {//top left corner
				nodeGrid[x][z].addEdge(&nodeGrid[x][z + 1], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x + 1][z], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x + 1][z + 1], 1);
			}
			else if (x - 1 < 0 && z + 1 >= std::size(nodeGrid)) {//top right corner
				nodeGrid[x][z].addEdge(&nodeGrid[x][z - 1], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x + 1][z], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x + 1][z - 1], 1);
			}
			else if (x + 1 >= std::size(nodeGrid) && z - 1 < 0) {//bottom left corner
				nodeGrid[x][z].addEdge(&nodeGrid[x][z + 1], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x - 1][z], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x - 1][z + 1], 1);
			}
			else if (x + 1 >= std::size(nodeGrid) && z + 1 >= std::size(nodeGrid)) {//bottom right corner
				nodeGrid[x][z].addEdge(&nodeGrid[x][z - 1], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x - 1][z], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x - 1][z - 1], 1);
			}
			else if (x - 1 < 0) {//left edge
				nodeGrid[x][z].addEdge(&nodeGrid[x - 1][z], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x + 1][z], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x][z + 1], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x - 1][z + 1], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x + 1][z + 1], 1);
			}
			else if (x + 1 >= std::size(nodeGrid)) {//right edge
				nodeGrid[x][z].addEdge(&nodeGrid[x - 1][z], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x + 1][z], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x][z - 1], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x - 1][z - 1], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x + 1][z - 1], 1);
			}
			else if (z - 1 < 0) {//top edge
				nodeGrid[x][z].addEdge(&nodeGrid[x][z - 1], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x][z + 1], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x + 1][z - 1], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x + 1][z], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x + 1][z + 1], 1);
			}
			else if (z + 1 >= std::size(nodeGrid)) {//bottom edge
				nodeGrid[x][z].addEdge(&nodeGrid[x][z - 1], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x][z + 1], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x - 1][z - 1], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x - 1][z], 1);
				nodeGrid[x][z].addEdge(&nodeGrid[x - 1][z + 1], 1);
			}
			else {//somewhere in the middle
				for (int row = -1; row <= 1; ++row) {
					for (int col = -1; col <= 1; ++col) {
						if (row == 0 && col == 0) continue;//ignore self
						if (nodeGrid[x + row][z + col].isObstructed()) {
							continue;
						}
						else {
							nodeGrid[x][z].addEdge(&nodeGrid[x + row][z + col], 1);
						}
					}
				}
			}
			//calculate the cost of travel, currently 1 or 0
			//update the nodes edges vector
		}
	}
	return nodeGrid;
}