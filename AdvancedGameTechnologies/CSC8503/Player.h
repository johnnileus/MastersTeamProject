#pragma once
#include "GameObject.h"
#include "GameTechRenderer.h"
#include "Window.h"
#include "GameWorld.h"
#include "Quaternion.h"
#include "ThirdPersonCamera.h"

namespace NCL {
	namespace CSC8503 {
		class Player : public GameObject
		{
		public:
			
			Player();
			~Player();

			GameObject* playerObject;
			
			void Update(float dt);
			void Init(ThirdPersonCamera* cam);

			void OnCollisionBegin(GameObject* otherObject) override;
			void OnCollisionEnd(GameObject* otherObject) override;
			void SetTemporaryColour(const Vector4& colour, float duration);
			static Player* Instantiate(GameWorld* world, ThirdPersonCamera* cam, const Vector3& position);

			GameWorld* myWorld;

			int score;

		protected:

			//component
			Mesh* playerMesh;
			Texture* playerTex;
			Shader* playerShader;
			PhysicsObject* playerPhysicObject;
			ThirdPersonCamera* myCam;

			//move
			float acceleratForce;
			float rotationFactor;
			float maxSpeed;
			float decelerationFactor;
			Vector2 inputDir;
			void HandleMovement(float dt,Vector2 inputDir);
			void HandleRotation(float dt);
			void ClampSpeed(float dt);
			void HandleInput();
			void SetComponent(float meshSize, float mass);

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
