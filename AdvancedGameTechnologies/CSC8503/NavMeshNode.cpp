#include "NavMeshNode.h"
#include "Ray.h"
#include "CollisionDetection.h"
#include "GameObject.h"

using namespace NCL;
using namespace CSC8503;

void NavMeshNode::checkObstructed() {
	NCL::Maths::Ray ray(this->position, NCL::Maths::Vector3(0, -1, 0));
	RayCollision collisionInfo;

	this->obstructed = false;
	bool hasHit = this->world->Raycast(ray, collisionInfo, true);
	if (hasHit) {
		GameObject* hitObject = (GameObject*)collisionInfo.node;
		if (hitObject && hitObject->tag == "Wall") {
			this->obstructed = true;
		}
	}
}

void NavMeshNode::AddEdge(NavMeshNode* neighbour, float cost) {
	Edge e;
	e.neighbour = neighbour;
	e.cost = cost;
	edges.push_back(e);
}

bool NavMeshNode::DetectImpassableEdge(NavMeshNode* neighbour) {
	float diffY = (neighbour->position.y - this->position.y);
	float diffX = (neighbour->position.x - this->position.x);
	float diffZ = (neighbour->position.z - this->position.z);
	return (diffY > diffX || diffY > diffZ);
}

void NavMeshNode::CalculateEdges() {
	//logic to determine the cost of a node to each neighbour
	for (int node = 0; node < std::size(edges); ++node) {
		if (DetectImpassableEdge(edges[node].neighbour)) {
			edges[node].cost = 1000000;
		}
		//logic for special terrain types to go here to adjust cost, e.g lava, ice, etc.
		else {
			edges[node].cost = 1;
		}
	}
}