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
	//calculate gradient between this node and neighbour
	//if the gradient is too steep, return true
	return false;
}

void NavMeshNode::calculateEdges() {
	//logic to determine the cost of a node to each neighbour
}