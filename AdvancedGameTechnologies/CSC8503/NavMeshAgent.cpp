#pragma once
#include "NavMeshAgent.h"
#include "NavMeshNode.h"
#include <cmath>

using namespace NCL;
using namespace CSC8503;

void NavMeshAgent::FindPath(NavMeshNode* currentNode, NavMeshNode* destination) {
	this->clearPath();
	currentNode->setGScore(0);
	currentNode->setHeuristic(calculateHeuristic(currentNode, destination));
	openList.emplace_back(currentNode);

	while (openList.size() > 0) {
		node = openList[0];
		for (int n = 1; n < openList.size(); ++n) {
			if (calculateFScore(openList[n]) < calculateFScore(node)) {
				node = openList[n];
			}
		}
		if (node == destination) {
			while (node) {
				path.emplace_back(node);
				//node = node->getParent();
			}
			//reverse path vector
			return;
		}
	}

	for (int e = 0; e < node->GetEdges().size(); ++e) {
		//if node.GetEdges()[e] is in closed list{
			//continue;}


		//potentially need to rewrite these functions to take the edge struct from NavMeshNode instead of the node
		newHeuristic = calculateHeuristic(node->GetEdges()[e].neighbour, destination);
		newGScore = calculateGScore(node->GetEdges()[e].neighbour);
		newFScore = calculateFScore(node->GetEdges()[e].neighbour);

		if (newFScore < calculateFScore(node)) {
			node->GetEdges()[e].neighbour->setGScore(newGScore);
			node->GetEdges()[e].neighbour->setFScore(newFScore);
			//node->GetEdges()[e].neighbour->SetParent(node);
		}
	}
	//remove node from open list
	closedList.emplace_back(node);
}

void NavMeshAgent::clearPath() {
	openList.clear();
	closedList.clear();
	path.clear();
}
