#include "GameObject.h"
#include "Player.h"

#include "Animator.h"
#include "AssetManager.h"
#include "Window.h"
#include "Enemy.h"
#include "PhysicsObject.h"
#include "Pistol.h"
#include "RenderObject.h"
#define FMT_LOCALE 0
#define FMT_STATIC_THOUSANDS_SEPARATOR
#define FMT_HEADER_ONLY 1
#include <fmt/core.h>


using namespace NCL;
using namespace CSC8503;
//w = (NCL::PS5::PS5Window*)Window::GetWindow();


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
void Player::Init(ThirdPersonCamera* cam)
{
	playerObject=this;
	maxSpeed = 10;
	score = 0;
	acceleratForce = 10;
	rotationFactor = 40;
	jumpForce = 50;
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
	playerPhysicObject = this->GetPhysicsObject();
	myCam=cam;
	

	myWeapon = new Pistol(this);
}

void Player::SetComponent(float meshSize,float mass)
{
	myMesh = AssetManager::Instance().roleMesh;
	//Collider
	SphereVolume* volume  = new SphereVolume(1);
	SetBoundingVolume((CollisionVolume*)volume);

	//Transform
	Transform& objectTransform = GetTransform();
	GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize));

	//Physics
	SetPhysicsObject(new PhysicsObject(&objectTransform, GetBoundingVolume()));
	GetPhysicsObject()->SetInverseMass(mass);
	GetPhysicsObject()->InitSphereInertia();
	
	//Render
	 SetRenderObject(new RenderObject(
	 	&objectTransform,
	 	myMesh,
	 	AssetManager::Instance().playerTex[1],
	 	AssetManager::Instance().characterShader)
	 	);
	renderObject->SetMaterial(AssetManager::Instance().guardMat);
	
	
	animator = new Animator(renderObject);
	animator->LoadAnimation("Role_Walk");
}

void ApplyBoneTransformsToModel(const std::vector<Maths::Matrix4>& boneTransforms, Mesh* mesh) {
	for (size_t i = 0; i < boneTransforms.size(); ++i) {
		 //mesh->SetBindPose(i, boneTransforms[i]);
	}
}


/// Create a player instance in world
/// @param world the scene
/// @param camera his camera
/// @param position instantiate position
/// @return 
#ifdef USEAGC
Player* Player::Instantiate(GameWorld* world, ThirdPersonCamera* camera, const Vector3& position, NCL::PS5::PS5Controller& controller)
#else
Player* Player::Instantiate(GameWorld* world, ThirdPersonCamera* camera, const Vector3& position)
#endif
{
	Player* player = new Player();

	// Set it's location and rotation
	player->GetTransform().SetPosition(position);
	player->GetTransform().SetOrientation(Quaternion());
	player->SetComponent(2,5);
	player->playerObject =player;
	player->myWorld = world;
	player->Init(camera);
#ifdef USEAGC
	player->inputController = &controller;
#endif // USEAGC

	// Add to the GameWorld
	if (world) {
		std::cout << "Adding player to world" << std::endl;
		world->AddGameObject(player);
	}
	camera->SetFollowObject(player);
	
	player-> animator->Play("Role_Walk",true,1);
	
	return player;
}


void Player::Update(float dt) {
	HandleDash(dt);  // Dash logic takes priority
	if (!isDashing || !isDead) {
		HandleMovement(dt,inputDir);  // Can only move normally when not dashing
	}
	HandleRotation(dt);
	HandleJump();
	HandleAim();
	DisplayUI();
	HealthCheck();
	
	animator->Update(dt);	
#ifdef USEAGC
	myWeapon->Update(dt, inputController->GetNamedButton("R2"), aimDir);
#else
	myWeapon->Update(dt, Window::GetMouse()->ButtonDown(MouseButtons::Left), aimDir);
#endif // USEAGC

	
	

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
	animator->Update(dt);
}

void Player::HealthCheck()
{
	if (health<=0)
	{
		Debug::Print("Dead", Vector2(40,40),Vector4(1,0,0,1));
		isDead = true;
	}
}

void Player::HandleInput()
{
    // each frame clear the input buffer
    inputDir = Vector2(0, 0);

#ifdef USEAGC
	// detect controller input
	if (inputController) {
		float leftStickX = inputController->GetAxis(0);
		float leftStickY = inputController->GetAxis(1);

		// map controller stick input to inputDir
		inputDir.x += leftStickX;
		inputDir.y += leftStickY;  // assuming up on the stick is negative Y // and thus I've reinvented tank controls
	}
	/*if (inputController->GetNamedButton("L2")) {
		Debug::Print("L2 pressed", Vector2(40, 40), Vector4(1, 0, 0, 1));
	}
	if (inputController->GetNamedButton("R2")) {
		Debug::Print("R2 pressed", Vector2(40, 40), Vector4(1, 0, 0, 1));
	}
	if (inputController->GetNamedButton("L1")) {
		Debug::Print("L1 pressed", Vector2(40, 40), Vector4(1, 0, 0, 1));
	}
	if (inputController->GetNamedButton("R1")) {
		Debug::Print("R1 pressed", Vector2(40, 40), Vector4(1, 0, 0, 1));
	}*/
#elif _WIN32
    // detect keyboard input
    if (Window::GetKeyboard()->KeyDown(KeyCodes::W)) {
        inputDir.y += -1.0f;  // front
    }
    if (Window::GetKeyboard()->KeyDown(KeyCodes::S)) {
        inputDir.y += 1.0f;   // back
    }
    if (Window::GetKeyboard()->KeyDown(KeyCodes::A)) {
        inputDir.x += -1.0f;  // left
    }
    if (Window::GetKeyboard()->KeyDown(KeyCodes::D)) {
        inputDir.x += 1.0f;   // right
    }

#endif // USEAGC
}

void Player::HandleMovement(float dt, Vector2 inputDir) {
	if (!playerPhysicObject) return;

	HandleInput();
	
	Vector3 levelCamFront = Vector3(myCam->front.x,0,myCam->front.z);
	Vector3 levelCamRight = Vector3(myCam->right.x,0,myCam->right.z);
	
	moveDir= Vector::Normalise(-levelCamFront * inputDir.y + levelCamRight * inputDir.x);
	Debug::DrawLine(transform.GetPosition(),transform.GetPosition()+moveDir);
	
	//if there has input value, add force
	if (Vector::Length(moveDir)>0) {
		Vector3 force = Vector::Normalise(moveDir) * acceleratForce;
		playerPhysicObject->AddForce(force);  //add force to object
	}
	ClampSpeed(dt);
}

//limit speed 
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

	if (Vector::Length(inputDir)<0.1)
	{
		playerPhysicObject->SetLinearVelocity(Vector3(0,0,0));
	}
}


void Player::HandleRotation(float dt) {
	
	Vector3 horizontalDir = Vector3(moveDir.x, 0.0f, moveDir.z);
	float length = Vector::Length(horizontalDir);

	// if no input direction dont rotate 
	if (length < 0.01f) {
		return;
	}
	Vector::Normalise(horizontalDir);

	// calculate rotation degrees
	float yawRadians = std::atan2(horizontalDir.x, horizontalDir.z);
	float yawDegrees = Maths::RadiansToDegrees(yawRadians);

	
	Quaternion desiredOrientation = Quaternion::EulerAnglesToQuaternion(0.0f, yawDegrees+180.0f, 0.0f);
	
	Quaternion currentOrientation = playerObject->GetTransform().GetOrientation();

	
	float dot = Quaternion::Dot(currentOrientation, desiredOrientation);
	if (dot < 0.0f) {
		desiredOrientation = -desiredOrientation;
	}

	// rotate speed
	float rotateSpeed = 15.0f;
	float t = rotateSpeed * dt;

	//smooth rotation
	Quaternion finalOrientation = Quaternion::Slerp(
		currentOrientation,
		desiredOrientation,
		t
	);
	
	playerObject->GetTransform().SetOrientation(finalOrientation);
}

void Player::HandleFire(float dt)
{
#ifdef USEAGC
	if (inputController->GetNamedButton("R2"))
#else
	if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::Left))
#endif // USEAGC
	{
		Debug::Print("Fire", Vector2(40, 40), Vector4(1, 0, 0, 1));
		myWeapon->Fire();
	}
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
#ifdef USEAGC
	if (inputController->GetNamedButton("Circle")) {
#else
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::SHIFT)) {
#endif // USEAGC
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
	if (!playerPhysicObject || !isOnGround) {
		return;
	}// Can only jump when on the ground
#ifdef USEAGC
	if (inputController->GetNamedButton("Cross")) {
		Debug::Print("Jump", Vector2(40, 40), Vector4(1, 0, 0, 1));
#else
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE)) {
#endif // USEAGC
		
		Vector3 jump = Vector3(0, jumpForce, 0);  // Apply upward force
		playerPhysicObject->AddForce(jump);
		isOnGround = false;  // Mark as off the ground
	}

}

void Player::HandleAim()
{
	Ray ray = CollisionDetection::BuildRayFromCamera(myWorld->GetMainCamera(), 100.0f, *myCam);
	
	RayCollision collisionInfo;
	bool hasHit = myWorld->Raycast(ray, collisionInfo, true);
	
	Vector3 hitPoint;
	if (hasHit) {
		hitPoint = collisionInfo.collidedAt; 
	} else {
		hitPoint = ray.GetPosition() + ray.GetDirection() * 100.0f;
	}
	aimDir = hitPoint - transform.GetPosition();
	Debug::DrawLine(transform.GetPosition(), hitPoint, Debug::RED);
	
}

Vector3 CollisionDetection::GetRayHitPoint(const Ray& ray)
{

	Vector3 planeNormal(0, 1, 0);
	float d = 0.0f;
    

	float denom = Vector::Dot(ray.GetDirection(), planeNormal);
    

	if (fabs(denom) > 1e-6)
	{
		
		float t = -(Vector::Dot(ray.GetPosition(), planeNormal) + d) / denom;
		if (t >= 0)
		{
			// 返回交点
			return ray.GetPosition() + ray.GetDirection() * t;
		}
	}
    
	
	return Vector3(-1, -1, -1);
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
#ifdef USEAGC
			Enemy* enemy = static_cast<Enemy*>(otherObject);
#else
			Enemy* enemy = dynamic_cast<Enemy*>(otherObject);
#endif // USEAGC
			score+=15;
			SetTemporaryColour(attackColour,0.25f);
			enemy->Reset();
		}
		else
		{
			health-=damage;
#ifdef USEAGC
			Enemy* enemy = static_cast<Enemy*>(otherObject);
#else
			Enemy* enemy = dynamic_cast<Enemy*>(otherObject);
#endif // USEAGC
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
	Debug::Print("0",Vector2(50,50));
	float velocity = Vector::Length(playerPhysicObject->GetLinearVelocity());
	Debug::Print("V:" + fmt::format("{:.1f}", velocity), Vector2(5, 10));
	Debug::Print("HP:"+std::to_string(health), Vector2(5,15));
	Debug::Print("Score:"+std::to_string(score), Vector2(80,10));
}

void Player::RemoveObject(GameObject* gameObject)
{
	gameObject->GetTransform().SetPosition(Vector3(-1000,-1000,-1000));
}









