#pragma once
#include "NavMeshAgent.h"
#include "NavMeshNode.h"
#include <cmath>
#include "PhysicsObject.h"
#include <queue>
#include <unordered_set>
#include "Debug.h"
#include <algorithm>

using namespace NCL;
using namespace CSC8503;

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
    this->currentNode->SetFScore(this->currentNode->GetHeuristic());

    this->openList.emplace_back(this->currentNode);

    //prefers to take a diagonal less optimal path, spent 3 hours attempting to fix to no avail
    while (!this->openList.empty()) {

        NavMeshNode* node = openList[0];
        for (int i = 1; i < openList.size(); ++i) {
            if (openList[i]->GetFScore() < node->GetFScore())
            {
                node = openList[i];
            }
            if (node == this->destination) {
                while (node != nullptr) {
                    this->path.emplace_back(node);
                    node = node->GetParent();
                }

                std::reverse(this->path.begin(), this->path.end());
                std::cout << "Destination: " << destination->GetPosition().x << " " << destination->GetPosition().z << std::endl;
                return;
            }
        }
                    
        for (int e = 0; e < node->GetEdges().size(); ++ e) {

            NavMeshNode* neighbour = node->GetEdges()[e].neighbour;

            //slow
            if (std::find(this->closedList.begin(), this->closedList.end(), neighbour) != this->closedList.end()) {
                continue;
            }

            float tempHeuristic = calculateHeuristic(neighbour, destination);
            float tempGScore = node->GetGScore() + node->GetEdges()[e].cost;
            float tempFScore = tempHeuristic + tempGScore;

            bool inOpenList = std::find(this->openList.begin(), this->openList.end(), neighbour) != this->openList.end();

            if (!inOpenList || tempFScore < node->GetFScore()) {
                neighbour->SetParent(node);
                neighbour->SetGScore(tempGScore);
                neighbour->SetHeuristic(tempHeuristic);
                neighbour->SetFScore(tempFScore);
                openList.emplace_back(neighbour);
            }
        }
        //slow
        openList.erase(find(openList.begin(), openList.end(), node));
        closedList.emplace_back(node);
    }
    std::cout << "No path found" << std::endl;
}


void NavMeshAgent::clearPath() {
	this->openList.clear();
	this->closedList.clear();
	this->path.clear();
}

float NavMeshAgent::calculateHeuristic(NavMeshNode* node, NavMeshNode* destination) {
    NCL::Maths::Vector3 difference = node->GetPosition() - destination->GetPosition();
    return abs(std::sqrtf((difference.x * difference.x) + (difference.z * difference.z)));
}

float NavMeshAgent::calculateGScore(NavMeshNode* node, float edgeCost) {
	return node->GetGScore() + edgeCost;
}

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
            this->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
            this->nextNode = this->path.front();
        }
    }
    else if (!this->path.empty()) {
        this->nextNode = this->path.front();
    }
    for (int n = 0; n < size(path) - 1; ++n) {
        Debug::DrawLine(path[n]->GetPosition(), path[n + 1]->GetPosition(), Vector4(1, 0, 0, 0), 0.1f);
    }
    MoveTowardsNextNode();
}

//agent slides around and misses nodes?
//could also be due to the errors in the pathfinding route
void NavMeshAgent::MoveTowardsNextNode() {
    if (this->nextNode == nullptr) {
        return;
    }

    const auto& currPos = this->GetCurrentPosition();
    const auto& nextPos = this->nextNode->GetPosition();
    this->GetPhysicsObject()->AddForce(Vector::Normalise(Vector3((nextPos.x - currPos.x), 0, (nextPos.z - currPos.z))));
}

void NavMeshAgent::SetDestination() {
	this->destination = this->nodeGrid->GetAllNodes()[rand() % this->nodeGrid->GetAllNodes().size()];
    std::cout << this->destination->GetPosition().x << ", " << this->destination->GetPosition().z << std::endl;
}

void NavMeshAgent::SetDestinationNull() {
    this->destination = nullptr;
}