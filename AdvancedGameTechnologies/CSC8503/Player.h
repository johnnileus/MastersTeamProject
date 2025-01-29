#pragma once
#include "GameObject.h"
#include "GameTechRenderer.h"
#include "Window.h"
#include "GameWorld.h"
#include "Quaternion.h"

namespace NCL {
	namespace CSC8503 {
		class Player : public GameObject
		{
		public:
			
			Player();
			~Player();

			GameObject* playerObject;
			
			void Update(float dt);
			void Init();

			void OnCollisionBegin(GameObject* otherObject) override;
			void OnCollisionEnd(GameObject* otherObject) override;
			void SetTemporaryColour(const Vector4& colour, float duration);

			GameWorld* myWorld;

			int score;

		protected:
			Mesh* playerMesh;
			Texture* playerTex;
			Shader* playerShader;
			PhysicsObject* playerPhysicObject;

			//move
			float acceleratForce;
			float rotationFactor;
			float maxSpeed;
			float decelerationFactor;
			void HandleMovement(float dt);
			void HandleRotation(float dt);
			void ClampSpeed(float dt);

			//jump
			float jumpForce;         
			bool isOnGround;         
			void HandleJump();
			bool isAtApex;  // mark arrive high point
			float downwardForce; 


			//health
			int health;
			int damage; 

			//dash
			bool isDashing;           // Whether the player is dashing
			float dashCooldown;       // Dash lockout duration
			float dashForceMultiplier; // Force multiplier for dashing
			float dashTimer;          // Timer for tracking dash cooldown
			Vector4 dashColour;       // Colour effect during dashing
			void HandleDash(float dt);  // Dash logic handler


			Vector4 defaultColour;
			Vector4 damageColour;
			Vector4 attackColour;
			Vector4 collerctCoinColour;

			void RemoveObject(GameObject* gameObject);
			
			float colourTimer;              
			bool isTemporaryColourActive;   // Whether the temporary colour is active
			
			void DisplayUI();

			void HealthCheck();
			bool isDead;
			
		};
	}
}
