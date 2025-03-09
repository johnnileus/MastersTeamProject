#pragma once
#include "NavMeshAgent.h"
#include "NavMeshNode.h"
#include <cmath>

using namespace NCL;
using namespace CSC8503;

void NavMeshAgent::FindPath(NavMeshNode* currentNode, NavMeshNode* destination) {
	this->clearPath();
	currentNode->SetParent(nullptr);
	currentNode->SetGScore(0);
	currentNode->SetHeuristic(calculateHeuristic(currentNode, destination));
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
				node = node->GetParent();
			}
			std::reverse(path.begin(), path.end());
			return;
		}
	}

	for (int e = 0; e < node->GetEdges().size(); ++e) {
		bool inList = false;
		for (int i = 0; i < closedList.size(); ++i) {
			if (node->GetEdges()[e].neighbour == closedList[i]) {
				inList = true;
				break;
			}
		}
		if (inList) {
			continue;
		}

		//need to rewrite these functions to take the edge struct from NavMeshNode instead of the node
		newHeuristic = calculateHeuristic(node->GetEdges()[e].neighbour, destination);
		newGScore = calculateGScore(node->GetEdges()[e].neighbour);
		newFScore = calculateFScore(node->GetEdges()[e].neighbour);

		if (newFScore < calculateFScore(node)) {
			node->GetEdges()[e].neighbour->SetGScore(newGScore);
			node->GetEdges()[e].neighbour->SetFScore(newFScore);
			node->GetEdges()[e].neighbour->SetParent(node);
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

//Euclidean Distance
float NavMeshAgent::calculateHeuristic(NavMeshNode* node, NavMeshNode* destination) {
	NCL::Maths::Vector3 difference = node->GetPosition() - destination->GetPosition();
	return std::sqrt(abs((difference.x * difference.x) + (difference.y * difference.y) + (difference.z * difference.z)));
}

float NavMeshAgent::calculateGScore(NavMeshNode* node) {
	//return node->GetParent()->GetGScore + edgeCost;
	return 0;
}

//check that the correct values are being pulled here
float NavMeshAgent::calculateFScore(NavMeshNode* node) {
	return node->getGScore() + node->getHeuristic();
}

void NavMeshAgent::setCurrentNode() {
	//determine the nearest node to the agent
	//currentNode = nearestNode;
}