#pragma once
#include "NavMeshAgent.h"
#include "NavMeshNode.h"
#include <cmath>
#include "PhysicsObject.h"
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include "Debug.h"

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
    this->currentNode->SetFScore(this->currentNode->GetHeuristic());

    std::priority_queue<NavMeshNode*, std::vector<NavMeshNode*>, CompareFScore> openQueue;
    std::unordered_map<NavMeshNode*, float> openSet;
    std::unordered_set<NavMeshNode*> closedSet;

    openQueue.push(this->currentNode);
    openSet[this->currentNode] = this->currentNode->GetFScore();

    //generated path appears to be too long
    //need to fix the logic error causing this
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
            std::cout << "Destination: " << destination->GetPosition().x << " " << destination->GetPosition().z << std::endl;
            return;
        }

        closedSet.insert(node);

        for (const auto& edge : node->GetEdges()) {
            NavMeshNode* neighbor = edge.neighbour;
            if (closedSet.find(neighbor) != closedSet.end()) {
                continue;
            }

            float tentativeGScore = node->GetGScore() + edge.cost;
            bool inOpenSet = openSet.find(neighbor) != openSet.end();

            //might be overwriting parent of a node
            //not using F score here, probably the cause of my issues
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
//check this as it could be returning wrong values
float NavMeshAgent::calculateHeuristic(NavMeshNode* node, NavMeshNode* destination) {
    NCL::Maths::Vector3 difference = node->GetPosition() - destination->GetPosition();
    return std::abs(difference.x) + std::abs(difference.y) + std::abs(difference.z);
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
            this->nextNode = this->path.front();
        }
    }
    else if (!this->path.empty()) {
        this->nextNode = this->path.front();
    }
    for (int n = 0; n < size(path) - 1; ++n) {
        Debug::DrawLine(path[n]->GetPosition(), path[n + 1]->GetPosition(), Vector4(1, 0, 0, 0), 1.0f);
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
    this->GetPhysicsObject()->AddForce(Vector3((nextPos.x - currPos.x) * 0.05, 0, (nextPos.z - currPos.z) * 0.05));
}

void NavMeshAgent::SetDestination() {
	this->destination = this->nodeGrid->GetAllNodes()[rand() % this->nodeGrid->GetAllNodes().size()];
    std::cout << this->destination->GetPosition().x << ", " << this->destination->GetPosition().z << std::endl;
}

void NavMeshAgent::SetDestinationNull() {
    this->destination = nullptr;
}