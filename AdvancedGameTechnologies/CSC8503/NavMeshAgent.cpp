#pragma once
#include "NavMeshAgent.h"
#include "NavMeshNode.h"
#include <cmath>
#include "PhysicsObject.h"
#include <queue>
#include <unordered_set>

using namespace NCL;
using namespace CSC8503;

struct CompareFScore {
    bool operator()(NavMeshNode* a, NavMeshNode* b) {
        return a->GetFScore() > b->GetFScore();
    }
};

void NavMeshAgent::FindPath() {
    this->clearPath();
    if (currentNode == nullptr) {
        std::cout << "No Current Node" << std::endl;
        return;
    }
    if (destination == nullptr) {
        std::cout << "No Destination Node" << std::endl;
        return;
    }
    this->currentNode->SetParent(nullptr);
    this->currentNode->SetGScore(0);
    this->currentNode->SetHeuristic(calculateHeuristic(this->currentNode, this->destination));
    this->currentNode->SetFScore(this->currentNode->GetHeuristic()); // Set initial F score

    std::priority_queue<NavMeshNode*, std::vector<NavMeshNode*>, CompareFScore> openQueue;
    std::unordered_map<NavMeshNode*, float> openSet;
    std::unordered_set<NavMeshNode*> closedSet;

    openQueue.push(this->currentNode);
    openSet[this->currentNode] = this->currentNode->GetFScore();

    while (!openQueue.empty()) {
        NavMeshNode* node = openQueue.top();
        openQueue.pop();
        openSet.erase(node);

        if (node == this->destination) {
            while (node != nullptr) {
                this->path.emplace_back(node);
                node = node->GetParent();
            }
            std::reverse(this->path.begin(), this->path.end());
            return;
        }

        closedSet.insert(node);

        for (const auto& edge : node->GetEdges()) {
            NavMeshNode* neighbor = edge.neighbour;
            if (closedSet.find(neighbor) != closedSet.end()) {
                continue; // Ignore the neighbor which is already evaluated
            }

            float tentativeGScore = node->GetGScore() + edge.cost;
            bool inOpenSet = openSet.find(neighbor) != openSet.end();

            if (!inOpenSet || tentativeGScore < neighbor->GetGScore()) {
                neighbor->SetParent(node);
                neighbor->SetGScore(tentativeGScore);
                neighbor->SetHeuristic(calculateHeuristic(neighbor, this->destination));
                neighbor->SetFScore(neighbor->GetGScore() + neighbor->GetHeuristic());

                if (!inOpenSet) {
                    openQueue.push(neighbor);
                    openSet[neighbor] = neighbor->GetFScore();
                }
                else {
                    // Update the F score in the open set
                    openSet[neighbor] = neighbor->GetFScore();
                }
            }
        }
    }
    std::cout << "No path found" << std::endl;
}


void NavMeshAgent::clearPath() {
	this->openList.clear();
	this->closedList.clear();
	this->path.clear();
}

//Manhattan Distance
float NavMeshAgent::calculateHeuristic(NavMeshNode* node, NavMeshNode* destination) {
    NCL::Maths::Vector3 difference = node->GetPosition() - destination->GetPosition();
    return std::abs(difference.x) + std::abs(difference.y) + std::abs(difference.z);
}

float NavMeshAgent::calculateGScore(NavMeshNode* node, float edgeCost) {
	return node->GetGScore() + edgeCost;
}

//check that the correct values are being pulled here
float NavMeshAgent::calculateFScore(float heuristic, float gScore) {
	return heuristic + gScore;
}

void NavMeshAgent::FollowPath() {
    const auto& currentPosition = GetCurrentPosition();
    int roundedX = std::round(currentPosition.x);
    int roundedZ = std::round(currentPosition.z);

    if (this->currentNode == nullptr || this->currentNode->GetPosition().x != roundedX || this->currentNode->GetPosition().z != roundedZ) {
        for (auto& node : this->nodeGrid->GetAllNodes()) {
            if (node->GetPosition().x == roundedX && node->GetPosition().z == roundedZ) {
                this->currentNode = node;
                break;
            }
        }
    }

    if (this->currentNode == this->destination || this->destination == nullptr || this->path.empty()) {
        SetDestination();
        FindPath();
        return;
    }

    if (this->currentNode == this->nextNode && !this->path.empty()) {
        this->path.erase(this->path.begin());
        if (!this->path.empty()) {
            this->nextNode = this->path.front();
        }
    }
    else if (!this->path.empty()) {
        this->nextNode = this->path.front();
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

void NavMeshAgent::SetDestination() {
	//pick a random node
	this->destination = this->nodeGrid->GetAllNodes()[rand() % this->nodeGrid->GetAllNodes().size()];
}