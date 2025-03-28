#include "NavMeshGrid.h"
#include "NavMeshNode.h"
#include <vector>

using namespace NCL;
using namespace CSC8503;
using namespace std;
//generate navmeshgrid centred at 0,0,0
std::vector<std::vector<NavMeshNode*>> NavMeshGrid::GenerateNavMeshGrid(int levelSize) {
	vector<vector<NavMeshNode*>> nodeGrid;
	std::vector<NavMeshNode*> nodeRow;
	for (int x = 0; x < levelSize; ++x) {
		nodeRow.clear();
		for (int z = 0; z < levelSize; ++z) {
			NCL::Maths::Vector3 nodePosition = NCL::Maths::Vector3((x - levelSize/2), 10, (z - levelSize/2));//replace 500 with the height of the vertex for the node
			nodeRow.emplace_back(new NavMeshNode(nodePosition, this->world));
		}
		nodeGrid.emplace_back(nodeRow);
	}
    for (int x = 0; x < nodeGrid.size(); ++x) {
        for (int z = 0; z < nodeGrid.size(); ++z) {
            nodeGrid[x][z]->checkObstructed();
            if (nodeGrid[x][z]->isObstructed()) {
                continue;
            }
            if (x - 1 < 0 && z - 1 < 0) { //top left corner
                if (!nodeGrid[x][z + 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x][z + 1], 1);
                if (!nodeGrid[x + 1][z]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z], 1);
                if (!nodeGrid[x + 1][z + 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z + 1], 1);
            }
            else if (x - 1 < 0 && z + 1 >= nodeGrid.size()) { //top right corner
                if (!nodeGrid[x][z - 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x][z - 1], 1);
                if (!nodeGrid[x + 1][z]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z], 1);
                if (!nodeGrid[x + 1][z - 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z - 1], 1);
            }
            else if (x + 1 >= nodeGrid.size() && z - 1 < 0) { //bottom left corner
                if (!nodeGrid[x][z + 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x][z + 1], 1);
                if (!nodeGrid[x - 1][z]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z], 1);
                if (!nodeGrid[x - 1][z + 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z + 1], 1);
            }
            else if (x + 1 >= nodeGrid.size() && z + 1 >= nodeGrid.size()) { //bottom right corner
                if (!nodeGrid[x][z - 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x][z - 1], 1);
                if (!nodeGrid[x - 1][z]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z], 1);
                if (!nodeGrid[x - 1][z - 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z - 1], 1);
            }
            else if (z - 1 < 0) { //left edge
                if (!nodeGrid[x - 1][z]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z], 1);
                if (!nodeGrid[x + 1][z]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z], 1);
                if (!nodeGrid[x][z + 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x][z + 1], 1);
                if (!nodeGrid[x - 1][z + 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z + 1], 1);
                if (!nodeGrid[x + 1][z + 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z + 1], 1);
            }
            else if (z + 1 >= nodeGrid.size()) { //right edge
                if (!nodeGrid[x - 1][z]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z], 1);
                if (!nodeGrid[x + 1][z]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z], 1);
                if (!nodeGrid[x][z - 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x][z - 1], 1);
                if (!nodeGrid[x - 1][z - 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z - 1], 1);
                if (!nodeGrid[x + 1][z - 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z - 1], 1);
            }
            else if (x - 1 < 0) { //top edge
                if (!nodeGrid[x][z - 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x][z - 1], 1);
                if (!nodeGrid[x][z + 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x][z + 1], 1);
                if (!nodeGrid[x + 1][z - 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z - 1], 1);
                if (!nodeGrid[x + 1][z]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z], 1);
                if (!nodeGrid[x + 1][z + 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z + 1], 1);
            }
            else if (x + 1 >= nodeGrid.size()) { //bottom edge
                if (!nodeGrid[x][z - 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x][z - 1], 1);
                if (!nodeGrid[x][z + 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x][z + 1], 1);
                if (!nodeGrid[x - 1][z - 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z - 1], 1);
                if (!nodeGrid[x - 1][z]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z], 1);
                if (!nodeGrid[x - 1][z + 1]->isObstructed()) nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z + 1], 1);
            }
            else { //somewhere in the middle
                for (int row = -1; row <= 1; ++row) {
                    for (int col = -1; col <= 1; ++col) {
                        if (row == 0 && col == 0) continue; //ignore self
                        if (!nodeGrid[x + row][z + col]->isObstructed()) {
                            nodeGrid[x][z]->AddEdge(nodeGrid[x + row][z + col], 1);
                        }
                    }
                }
            }
		}
	}
	return nodeGrid;
}