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
			NCL::Maths::Vector3 nodePosition = NCL::Maths::Vector3(1* (x - levelSize/2), 10, 1*(z - levelSize/2));//replace 500 with the height of the vertex for the node
			nodeRow.emplace_back(new NavMeshNode(nodePosition));
		}
		nodeGrid.emplace_back(nodeRow);
	}
	for (int x = 0; x < nodeGrid.size(); ++x) {
		for (int z = 0; z < nodeGrid.size(); ++z) {
			nodeGrid[x][z]->checkObstructed();
            if (x - 1 < 0 && z - 1 < 0) {//top left corner
                nodeGrid[x][z]->AddEdge(nodeGrid[x][z + 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x][z + 1]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x + 1][z]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z + 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x + 1][z + 1]->GetPosition()));
            }
            else if (x - 1 < 0 && z + 1 >= nodeGrid.size()) {//top right corner
                nodeGrid[x][z]->AddEdge(nodeGrid[x][z - 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x][z - 1]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x + 1][z]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z - 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x + 1][z - 1]->GetPosition()));
            }
            else if (x + 1 >= nodeGrid.size() && z - 1 < 0) {//bottom left corner
                nodeGrid[x][z]->AddEdge(nodeGrid[x][z + 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x][z + 1]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x - 1][z]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z + 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x - 1][z + 1]->GetPosition()));
            }
            else if (x + 1 >= nodeGrid.size() && z + 1 >= nodeGrid.size()) {//bottom right corner
                nodeGrid[x][z]->AddEdge(nodeGrid[x][z - 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x][z - 1]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x - 1][z]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z - 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x - 1][z - 1]->GetPosition()));
            }
            else if (z - 1 < 0) {//left edge
                nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x - 1][z]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x + 1][z]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x][z + 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x][z + 1]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z + 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x - 1][z + 1]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z + 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x + 1][z + 1]->GetPosition()));
            }
            else if (z + 1 >= nodeGrid.size()) {//right edge
                nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x - 1][z]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x + 1][z]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x][z - 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x][z - 1]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z - 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x - 1][z - 1]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z - 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x + 1][z - 1]->GetPosition()));
            }
            else if (x - 1 < 0) {//top edge
                nodeGrid[x][z]->AddEdge(nodeGrid[x][z - 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x][z - 1]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x][z + 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x][z + 1]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z - 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x + 1][z - 1]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x + 1][z]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x + 1][z + 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x + 1][z + 1]->GetPosition()));
            }
            else if (x + 1 >= nodeGrid.size()) {//bottom edge
                nodeGrid[x][z]->AddEdge(nodeGrid[x][z - 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x][z - 1]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x][z + 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x][z + 1]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z - 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x - 1][z - 1]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x - 1][z]->GetPosition()));
                nodeGrid[x][z]->AddEdge(nodeGrid[x - 1][z + 1], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x - 1][z + 1]->GetPosition()));
            }
            else {//somewhere in the middle
                for (int row = -1; row <= 1; ++row) {
                    for (int col = -1; col <= 1; ++col) {
                        if (row == 0 && col == 0) continue;//ignore self
                        if (!nodeGrid[x + row][z + col]->isObstructed()) {
                            nodeGrid[x][z]->AddEdge(nodeGrid[x + row][z + col], CalculateDistance(nodeGrid[x][z]->GetPosition(), nodeGrid[x + row][z + col]->GetPosition()));
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

float  NavMeshGrid::CalculateDistance(const NCL::Maths::Vector3& pos1, const NCL::Maths::Vector3& pos2) {
    NCL::Maths::Vector3 difference = pos1 - pos2;
    return std::abs(std::sqrtf((difference.x * difference.x) + (difference.z * difference.z)));
}