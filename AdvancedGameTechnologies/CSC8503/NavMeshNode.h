#pragma once
#include <vector>
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class NavMeshNode;

		struct Edge {
			NavMeshNode* neighbour;
			float cost;
		};

		class NavMeshNode {
		public:
			NavMeshNode(NCL::Maths::Vector3 position, GameWorld* world) {
				this->position = position;
				this->obstructed = false;
				this->gScore = 0;
				this->fScore = 0;
				this->heuristic = 0;
				this->parent = nullptr;
				this->world = world;
			}
			void checkObstructed();
			bool isObstructed() {
				return obstructed;
			}
			bool DetectImpassableEdge(NavMeshNode* neighbour);
			void CalculateEdges();
			void AddEdge(NavMeshNode* neighbour, float cost);
			std::vector<Edge> GetEdges() { return this->edges; }
			NCL::Maths::Vector3 GetPosition() {	return this->position; }
			float GetGScore(){ return this->gScore; }
			float GetFScore(){ return this->fScore; }
			float GetHeuristic(){ return this->heuristic; }
			void SetGScore(float gScore) { this->gScore = gScore; }
			void SetFScore(float fScore) { this->fScore = fScore; }
			void SetHeuristic(float heuristic) { this->heuristic = heuristic; }
			void SetParent(NavMeshNode* parent) { this->parent = parent; }
			NavMeshNode* GetParent() { return this->parent; }
			bool IsNodeInUse() { return false; }
		protected:
			std::vector<Edge> edges;
			NCL::Maths::Vector3 position;
			NavMeshNode* parent;
			bool obstructed;
			float gScore;
			float fScore;
			float heuristic;
			GameWorld* world;
		};
	}
}