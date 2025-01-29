#pragma once

#include "GameObject.h"
#include "NavigationGrid.h"
#include "Player.h"

#include "Window.h"
#include "StateMachine.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

namespace NCL
{
    namespace CSC8503
    {
        class Enemy : public GameObject
        {
        public:
            Enemy(Player* player, GameWorld* world);
            ~Enemy();

            void Init();

            void SetMovePath(const std::vector<Vector3>& path); // 设置路径
            void Update(float dt);

            GameWorld* myWorld;
            void Reset();

        protected:

            float maxSpeed;
            float acceleratForce;

            void IdleState(float dt);    // Idle behavior
            void ChaseState(float dt);   // Chase behavior

            float decelerationForce; // 超过最大速度时的减速力
            float rotationFactor;
            void ClampSpeed(float dt);
            void HandleRotation(float dt);
            void FollowPath(float dt);

            StateMachine* stateMachine; // State machine for enemy behavior
            Player* targetPlayer;       // Reference to the player

            std::vector<Vector3> movePath;   // 外部传入的路径
            size_t currentNodeIndex;         // 当前路径节点索引
            float waypointThreshold = 1.0f;  // 距离阈值，用于判断是否到达节点
            
            
            Vector4 defaultColour;
            Vector4 hitColour;
        };
    }
}
