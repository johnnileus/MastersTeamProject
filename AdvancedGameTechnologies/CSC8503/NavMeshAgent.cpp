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

    this->openList.insert(this->currentNode);

    while (!this->openList.empty()) {
        // Find the node with the lowest F score
        auto it = std::min_element(openList.begin(), openList.end(), [](NavMeshNode* a, NavMeshNode* b) {
            return a->GetFScore() < b->GetFScore();
            });
        NavMeshNode* node = *it;

        // If the destination has been found, follow the node through its parents to find the best path
        if (node == this->destination) {
            while (node != nullptr) {
                this->path.emplace_back(node);
                node = node->GetParent();
            }
            std::reverse(this->path.begin(), this->path.end());
            std::cout << "Destination: " << destination->GetPosition().x << " " << destination->GetPosition().z << std::endl;
            return;
        }

        // Remove the node from the open list and add it to the closed list
        openList.erase(it);
        closedList.insert(node);

        // Iterate through each neighbor
        for (int e = 0; e < node->GetEdges().size(); ++e) {
            NavMeshNode* neighbour = node->GetEdges()[e].neighbour;

            // If the node has been checked, skip it
            if (closedList.find(neighbour) != closedList.end()) {
                continue;
            }

            // Calculate H, G, and F
            float tempHeuristic = calculateHeuristic(neighbour, destination);
            float tempGScore = node->GetGScore() + node->GetEdges()[e].cost;
            float tempFScore = tempHeuristic + tempGScore;

            // Check if the node is already in the open list
            bool inOpenList = openList.find(neighbour) != openList.end();

            // Update the parent node if the new path is more optimal
            if (!inOpenList || tempFScore < neighbour->GetFScore()) {
                neighbour->SetParent(node);
                neighbour->SetGScore(tempGScore);
                neighbour->SetHeuristic(tempHeuristic);
                neighbour->SetFScore(tempFScore);
                if (!inOpenList) {
                    openList.insert(neighbour);
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

void NavMeshAgent::setCurrentNode(int x, int z) {
    for (auto& node : this->nodeGrid->GetAllNodes()) {
        if (node->GetPosition().x == x && node->GetPosition().z == z) {
            this->currentNode = node;
            break;
        }
    }
}

void NavMeshAgent::FollowPath() {
    const auto& currentPosition = GetCurrentPosition();
    int roundedX = std::round(currentPosition.x);
    int roundedZ = std::round(currentPosition.z);
    
    if (this->currentNode == nullptr || this->currentNode->GetPosition().x != roundedX || this->currentNode->GetPosition().z != roundedZ) {
        setCurrentNode(roundedX, roundedZ);
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
        for (int n = 0; n < size(path) - 1; ++n) {
            Debug::DrawLine(path[n]->GetPosition(), path[n + 1]->GetPosition(), Vector4(1, 0, 0, 0), 0.1f);
        }
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
    this->GetPhysicsObject()->AddForce(Vector::Normalise(Vector3((nextPos.x - currPos.x), 0, (nextPos.z - currPos.z))) * Vector3(50,50,50));
}

void NavMeshAgent::SetDestination() {
	this->destination = this->nodeGrid->GetAllNodes()[rand() % this->nodeGrid->GetAllNodes().size()];
    std::cout << this->destination->GetPosition().x << ", " << this->destination->GetPosition().z << std::endl;
}

void NavMeshAgent::SetDestinationNull() {
    this->destination = nullptr;
}

void NavMeshAgent::Spawn() {
    this->spawnPosition = this->nodeGrid->GetAllNodes()[rand() % this->nodeGrid->GetAllNodes().size()]->GetPosition();
    this->spawnPosition = Vector3(this->spawnPosition.x, 0, this->spawnPosition.z);
    this->currentHealth = this->maxHealth;
    this->alive = true;
    this->GetTransform().SetPosition(this->spawnPosition);
    this->GetPhysicsObject();//need a function to make physics objects active
    std::cout << "Enemy Spawned";
}