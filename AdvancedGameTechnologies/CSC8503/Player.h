#pragma once
#include "Animator.h"
#include "GameObject.h"
#include "GameTechRenderer.h"
#include "Window.h"
#include "GameWorld.h"
#include "Quaternion.h"
#include "ThirdPersonCamera.h"
#include "Weapon.h"
#include <fmod.hpp>


namespace NCL {
	namespace CSC8503 {

		class Pistol;
		class Rifle;
		class  Shotgun;
		
		class Player : public GameObject
		{
		public:
			
			Player();
			~Player();

			GameObject* playerObject;
			
			void Update(float dt);
			void PausedUpdate(float dt);
			void Init(ThirdPersonCamera* cam);

			void OnCollisionBegin(GameObject* otherObject) override;
			void OnCollisionEnd(GameObject* otherObject) override;
			void SetTemporaryColour(const Vector4& colour, float duration);
#ifdef USEAGC
			static Player* Instantiate(GameWorld* world, ThirdPersonCamera* cam, const Vector3& position, NCL::PS5::PS5Controller& controller);
#else
			static Player* Instantiate(GameWorld* world, ThirdPersonCamera* cam, const Vector3& position);

#endif // USEAGC

			int GetHealth() {
				return health;
			}

			GameWorld* myWorld;

			int score;

			ThirdPersonCamera* myCam;
			//Weapon
			Weapon* currentWeapon;
			Pistol* pistol;
			Rifle* rifle;
			Shotgun* shotGun;
			vector<Weapon*> weaponPack;
			Vector3 shootPoint;

			Event<Player*> OnSwitchWeaponEvent;
			

		protected:

			//component
			Mesh* playerMesh;
			Shader* playerShader;
			PhysicsObject* playerPhysicObject;
			Transform* renderOffsetTransform;
			

			//move
			float acceleratForce;
			float rotationFactor;
			float maxSpeed;
			float decelerationFactor;
			Vector2 inputDir;
			Vector3 moveDir;
			Vector3 aimDir;
			void HandleMovement(float dt,Vector2 inputDir);
			void HandleRotation(float dt);
			void HandleFire(float dt);
			void HandleAim();
			void ClampSpeed(float dt);
			void HandleInput();
			void SetComponent(float meshSize, float mass);
			void FaceAimDirection(float dt);

			//jump
			float jumpForce;         
			bool isOnGround;         
			void HandleJump(float dt);
			void UpdateGroundStatus();
			void FixBounce();
			float jumpTimeCounter;
			bool wasOnGround = false;
			
			
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

			//Graphic
			Animator* animator;
			Mesh* myMesh;
			Vector4 defaultColour;
			Vector4 damageColour;
			Vector4 attackColour;
			Vector4 collerctCoinColour;

			void HandleSwitchWeapon();
			void RegisterWeaponEvents();

			void RemoveObject(GameObject* gameObject);
			
			float colourTimer;              
			bool isTemporaryColourActive;   // Whether the temporary colour is active
			
			void DisplayUI();

			void HealthCheck();
			bool isDead;
			FMOD::Channel* footstepChannel = nullptr;
#ifdef USEAGC
			NCL::PS5::PS5Controller* inputController;
#endif // USEAGC
			
		};
	}
}
