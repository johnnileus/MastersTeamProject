#include "GameObject.h"
#include "Player.h"

#include "Window.h"
#include "TutorialGame.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TutorialGame.h"

using namespace NCL;
using namespace CSC8503;



Player::Player()
{
	name = "player";
	tag = "Player";
	if (name == "player")
	{
		health = 100;
		damage =30;
	}
}

//init after player has get "object" in tutorialGame
void Player::Init()
{
	playerObject=this;
	maxSpeed = 10;
	score = 0;
	acceleratForce = 10;
	rotationFactor = 40;
	jumpForce = 25;
	isAtApex = false;
	downwardForce = 20.0f; //gravity
	isOnGround = true;
	isDashing=false;
	isDead = false;
	dashCooldown=0.5f;
	dashForceMultiplier=20;
	decelerationFactor =18;
	dashTimer = 0.0f;
	dashColour =Vector4(0.259f,0.96f,1,1);
	defaultColour = Vector4(1,1,1,1);
	damageColour = Vector4(1,0.29f,0,1);
	attackColour = Vector4(0.18f, 1,0,1);
	collerctCoinColour = Vector4(0.949f,1,0.318f,1);
	myWorld=new GameWorld();
	playerPhysicObject = this->GetPhysicsObject();
	
}


void Player::Update(float dt) {
	HandleDash(dt);  // Dash logic takes priority
	if (!isDashing || !isDead) {
		HandleMovement(dt);  // Can only move normally when not dashing
	}
	HandleRotation(dt);
	HandleJump();
	DisplayUI();
	HealthCheck();

	// Colour change timer
	if (isTemporaryColourActive) {
		colourTimer -= dt;
		if (colourTimer <= 0.0f) {
			// Restore default colour
			this->GetRenderObject()->SetColour(defaultColour);
			isTemporaryColourActive = false; // Deactivate temporary colour
		}
	}


	// check if arrive the top point
	if (!isOnGround && playerPhysicObject) {
		Vector3 velocity = playerPhysicObject->GetLinearVelocity();
		if (!isAtApex && velocity.y <= 0) {  
			isAtApex = true;  // mark
			playerPhysicObject->AddForce(Vector3(0, -downwardForce, 0)); 
		}
	}
	if (isOnGround) {
		isAtApex = false;  
	}
	
}

void Player::HealthCheck()
{
	if (health<=0)
	{
		Debug::Print("Dead", Vector2(40,40),Vector4(1,0,0,1));
		isDead = true;
	}
}


void Player::HandleMovement(float dt) {
	if (!playerPhysicObject) return;
	Vector3 movement(0, 0, 0);

	// detect keyboard input
	if (Window::GetKeyboard()->KeyDown(KeyCodes::W)) {
		movement.z += -1.0f;  // front
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::S)) {
		movement.z += 1.0f;   // back
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::A)) {
		movement.x += -1.0f;  // left
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::D)) {
		movement.x += 1.0f;   // right
	}

	//if there has input value, add force
	if (Vector::Length(movement)>0) {
		Vector3 force = Vector::Normalise(movement) * acceleratForce;
		playerPhysicObject->AddForce(force);  //add force to object
	}
	ClampSpeed(dt);
}

//limit speed to the max speed
void Player::ClampSpeed(float dt) {
	if (!playerPhysicObject) return;

	Vector3 velocity = playerPhysicObject->GetLinearVelocity();
	float speed = Vector::Length(velocity);

	if (speed > maxSpeed) {
		// Calculate deceleration direction (opposite to velocity direction)
		Vector3 deceleration = -Vector::Normalise(velocity) * decelerationFactor;

		// If the speed still exceeds maxSpeed after deceleration, continue applying deceleration force
		if (Vector::Length(playerPhysicObject->GetLinearVelocity()) > maxSpeed) {
			playerPhysicObject->AddForce(deceleration);
		} else {
			Vector3 limitedVelocity = Vector::Normalise(velocity) * maxSpeed;
			playerPhysicObject->SetLinearVelocity(limitedVelocity);
		}

	}
}


void Player::HandleRotation(float dt) {
	if (!playerPhysicObject) return;

	// Get the ball's linear velocity
	Vector3 velocity = playerPhysicObject->GetLinearVelocity();

	// If the speed is close to zero, do not rotate
	if (Vector::Length(velocity) < 0) return;

	// Calculate the rotation axis: perpendicular to the velocity direction (right-hand rule)
	Vector3 rotationAxis = Vector::Normalise(Vector3(velocity.z, 0, -velocity.x));

	// Calculate the angular speed: linear speed divided by the ball's radius
	float angularSpeed = Vector::Length(velocity) / playerObject->GetTransform().GetScale().x * rotationFactor;  // Radius is determined by the scale factor

	// Calculate the rotation angle
	float rotationAngle = angularSpeed * dt;

	// Generate a quaternion representing the rotation
	Quaternion rotation = Quaternion::AxisAngleToQuaterion(rotationAxis, rotationAngle);

	// Apply the rotation to the ball's transform
	playerObject->GetTransform().SetOrientation(rotation * playerObject->GetTransform().GetOrientation());

}

void Player::HandleDash(float dt) {
	if (isDashing) {
		if (!isTemporaryColourActive)
		{
			renderObject->SetColour(dashColour);
		}
		dashTimer -= dt;  // countdown timmer
		if (dashTimer <= 0.0f) {
			isDashing = false;  // Dash ends, restore normal control

			renderObject->SetColour(defaultColour);
		}
		return;  // Ignore other inputs during dashing

	}

	// if press Shift
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::SHIFT)) {
		Vector3 velocity = playerPhysicObject->GetLinearVelocity();
		if (Vector::Length(velocity) > 0) {  // If the ball has velocity
			Vector3 dashForce = Vector::Normalise(velocity) * acceleratForce * dashForceMultiplier;
			playerPhysicObject->AddForce(dashForce);  // Dash direction follows the current velocity
			isDashing = true;
			dashTimer = dashCooldown;  // Start cooldown timer
		}

	}
}


void Player::HandleJump() {
	if (!playerPhysicObject || !isOnGround) return;  // Can only jump when on the ground

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE)) {
		Vector3 jump = Vector3(0, jumpForce, 0);  // Apply upward force
		playerPhysicObject->AddForce(jump);
		isOnGround = false;  // Mark as off the ground
	}

}

void Player::OnCollisionBegin(GameObject* otherObject)
{
	//collides ground
	if (otherObject->tag=="Ground")
	{
		isOnGround =true;
	}

	//collides Enemy
	if (otherObject->tag=="Enemy")
	{
		std::cout<<otherObject->GetName()<<std::endl;
		Debug::DrawLine(this->GetTransform().GetPosition(),otherObject->GetTransform().GetPosition());
		if (isDashing)
		{
			Enemy* enemy = dynamic_cast<Enemy*>(otherObject);
			score+=15;
			SetTemporaryColour(attackColour,0.25f);
			enemy->Reset();
		}
		else
		{
			health-=damage;
			Enemy* enemy = dynamic_cast<Enemy*>(otherObject);
			SetTemporaryColour(damageColour,0.25f);
			enemy->Reset();

		}
	}

	//collides Coin
	if (otherObject->tag=="Coin")
	{
		score+=10;
		otherObject->SetActive(false);
		RemoveObject(otherObject);
		SetTemporaryColour(collerctCoinColour,0.25f);
	}
}

void Player::OnCollisionEnd(GameObject* otherObject)
{

}

void Player::SetTemporaryColour(const Vector4& colour, float duration) {

	colourTimer = duration;                
	isTemporaryColourActive = true;        // active timer
	this->GetRenderObject()->SetColour(colour);
}


void Player::DisplayUI()
{
	
	float velocity = Vector::Length(playerPhysicObject->GetLinearVelocity());
	Debug::Print("V:" + std::format("{:.1f}", velocity), Vector2(5, 10));
	Debug::Print("HP:"+std::to_string(health), Vector2(5,15));
	Debug::Print("Score:"+std::to_string(score), Vector2(80,10));
}

void Player::RemoveObject(GameObject* gameObject)
{
	gameObject->GetTransform().SetPosition(Vector3(-1000,-1000,-1000));
}









