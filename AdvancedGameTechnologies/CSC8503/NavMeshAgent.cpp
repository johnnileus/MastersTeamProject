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
	NavMeshNode* node = nullptr;

	while (openList.size() > 0) {
		node = openList[0];
		for (int n = 1; n < openList.size(); ++n) {
			if (openList[n]->GetFScore() < node->GetFScore()) {
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

		newHeuristic = calculateHeuristic(node->GetEdges()[e].neighbour, destination);
		newGScore = calculateGScore(node->GetEdges()[e].neighbour, node->GetEdges()[e].cost);
		newFScore = calculateFScore(newHeuristic, newGScore);

		if (newFScore < node->GetFScore()) {
			node->GetEdges()[e].neighbour->SetGScore(newGScore);
			node->GetEdges()[e].neighbour->SetFScore(newFScore);
			node->GetEdges()[e].neighbour->SetParent(node);
		}
	}
	path.erase(find(path.begin(), path.end(), node));
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
	return abs(std::sqrt((difference.x * difference.x) + (difference.y * difference.y) + (difference.z * difference.z)));
}

float NavMeshAgent::calculateGScore(NavMeshNode* node, float edgeCost) {
	return node->GetParent()->GetGScore() + edgeCost;
}

//check that the correct values are being pulled here
float NavMeshAgent::calculateFScore(float heuristic, float gScore) {
	return heuristic + gScore;
}

void NavMeshAgent::setCurrentNode() {
	int roundedX = std::round(GetCurrentPosition().x);
	int roundedZ = std::round(GetCurrentPosition().z);
	for (int n = 0; n < nodeGrid->GetAllNodes().size(); ++n) {
		if (this->nodeGrid->GetAllNodes()[n]->GetPosition().x == roundedX && this->nodeGrid->GetAllNodes()[n]->GetPosition().z == roundedZ) {
			this->currentNode = this->nodeGrid->GetAllNodes()[n];
			break;
		}
	}
}

void NavMeshAgent::FollowPath() {
	setCurrentNode();
	if (this->currentNode == this->destination) {
		return;
	}
	if (this->currentNode == this->nextNode) {
		this->path.erase(this->path.begin());
		this->nextNode = path[0];
	}
}