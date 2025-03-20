#pragma once
#include "NavMeshAgent.h"
#include "NavMeshNode.h"
#include <cmath>
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

void NavMeshAgent::FindPath() {
	this->clearPath();
	this->currentNode->SetParent(nullptr);
	this->currentNode->SetGScore(0);
	this->currentNode->SetHeuristic(calculateHeuristic(this->currentNode, this->destination));
	this->openList.emplace_back(this->currentNode);
	NavMeshNode* node = nullptr;

	while (this->openList.size() > 0) {
		node = this->openList[0];
		for (int n = 1; n < this->openList.size(); ++n) {
			if (this->openList[n]->GetFScore() < node->GetFScore()) {
				node = this->openList[n];
			}
		}
		if (node == this->destination) {
			while (node) {
				this->path.emplace_back(node);
				node = node->GetParent();
			}
			std::reverse(this->path.begin(), this->path.end());
			return;
		}
	}

	for (int e = 0; e < node->GetEdges().size(); ++e) {
		bool inList = false;
		for (int i = 0; i < this->closedList.size(); ++i) {
			if (node->GetEdges()[e].neighbour == this->closedList[i]) {
				inList = true;
				break;
			}
		}
		if (inList) {
			continue;
		}

		this->newHeuristic = calculateHeuristic(node->GetEdges()[e].neighbour, this->destination);
		this->newGScore = calculateGScore(node->GetEdges()[e].neighbour, node->GetEdges()[e].cost);
		this->newFScore = calculateFScore(this->newHeuristic, this->newGScore);

		if (this->newFScore < node->GetFScore()) {
			node->GetEdges()[e].neighbour->SetGScore(this->newGScore);
			node->GetEdges()[e].neighbour->SetFScore(this->newFScore);
			node->GetEdges()[e].neighbour->SetParent(node);
		}
	}
	this->path.erase(find(this->path.begin(), this->path.end(), node));
	this->closedList.emplace_back(node);
}

void NavMeshAgent::clearPath() {
	this->openList.clear();
	this->closedList.clear();
	this->path.clear();
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
		SetDestination();
		FindPath();
		return;
	}
	if (this->currentNode == this->nextNode) {
		this->path.erase(this->path.begin());
		this->nextNode = path[0];
	}
	MoveTowardsNextNode();
}

void NavMeshAgent::MoveTowardsNextNode() {
	float currX = this->currentNode->GetPosition().x;
	float currZ = this->currentNode->GetPosition().z;
	float nextX = this->nextNode->GetPosition().x;
	float nextZ = this->nextNode->GetPosition().z;
	this->GetPhysicsObject()->AddForce(Vector3(nextX - currX, 0, nextZ - currZ));
}