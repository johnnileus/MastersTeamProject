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

            void SetMovePath(const std::vector<Vector3>& path); // set the path
            void Update(float dt);

            GameWorld* myWorld;
            void Reset();

        protected:

            float maxSpeed;
            float acceleratForce;

            void IdleState(float dt);    // Idle behavior
            void ChaseState(float dt);   // Chase behavior

            float decelerationForce; // Deceleration force applied when exceeding max speed
            float rotationFactor;    // Factor influencing rotation speed

            void ClampSpeed(float dt);    // Limits the speed to prevent excessive velocity
            void HandleRotation(float dt); // Handles the enemy's rotation logic
            void FollowPath(float dt);    // Makes the enemy follow a predefined path

            StateMachine* stateMachine; // State machine for enemy behavior
            Player* targetPlayer;       // Reference to the player

            std::vector<Vector3> movePath;   // Externally provided movement path
            size_t currentNodeIndex;         // Current index in the path
            float waypointThreshold = 1.0f;  // Distance threshold to determine if a node is reached

            
            
            Vector4 defaultColour;
            Vector4 hitColour;
        };
    }
}
