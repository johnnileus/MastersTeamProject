#include "NavMeshNode.h"

using namespace NCL;
using namespace CSC8503;

void NavMeshNode::checkObstructed() {
	//logic to determine if a node is obstructed by an environment object

	this->obstructed = false;
}

void NavMeshNode::addEdge(NavMeshNode* neighbour, int cost) {
	Edge e;
	e.neighbour = neighbour;
	e.cost = cost;
	edges.push_back(e);
}

bool NavMeshNode::detectImpassableEdge(NavMeshNode* neighbour) {
	float diffY = (neighbour->position.y - this->position.y);
	float diffX = (neighbour->position.x - this->position.x);
	float diffZ = (neighbour->position.z - this->position.z);
	return (diffY > diffX || diffY > diffZ);
}

void NavMeshNode::calculateEdges() {
	//logic to determine the cost of a node to each neighbour
	for (int node = 0; node < std::size(edges); ++node) {
		if (detectImpassableEdge(edges[node].neighbour)) {
			edges[node].cost = 1000000;
		}
		//logic for special terrain types to go here to adjust cost, e.g lava, ice, etc.
		else {
			edges[node].cost = 1;
		}
	}
}