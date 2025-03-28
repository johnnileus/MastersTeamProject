#include "GameObject.h"
#include "Player.h"

#include "Animator.h"
#include "AssetManager.h"
#include "Window.h"
#include "Enemy.h"
#include "PhysicsObject.h"
#include "Pistol.h"
#include "Rifle.h"
#include "Shotgun.h"
#include "RenderObject.h"
#include "AudioManager.h"

#include "PassiveItem.h"

#include "Weapon.h"

using namespace NCL;
using namespace CSC8503;

Player::Player()
{
	name = "player";
	tag = "Player";
	if (name == "player")
	{
		health = 100;
		damage = 30;
	}
}

// init after player has get "object" in tutorialGame
void Player::Init(ThirdPersonCamera* cam)
{
	playerObject = this;
	maxSpeed = 10;
	score = 0;
	acceleratForce = 10;
	rotationFactor = 40;
	jumpForce = 700;
	isOnGround = true;
	isDashing = false;
	isDead = false;
	dashCooldown = 0.5f;
	dashForceMultiplier = 20;
	decelerationFactor = 18;
	dashTimer = 0.0f;
	dashColour = Vector4(0.259f, 0.96f, 1, 1);
	defaultColour = Vector4(1, 1, 1, 1);
	damageColour = Vector4(1, 0.29f, 0, 1);
	attackColour = Vector4(0.18f, 1, 0, 1);
	collerctCoinColour = Vector4(0.949f, 1, 0.318f, 1);
	playerPhysicObject = this->GetPhysicsObject();
	myCam = cam;
	pistol = new Pistol(this);
	rifle = new Rifle(this);
	shotGun = new Shotgun(this);
	currentWeapon = rifle;
	weaponPack = {pistol,rifle,shotGun};

	//////For weapon debug//////
	RegisterWeaponEvents();
	OnSwitchWeaponEvent.AddListener([](Player* m){
		m->RegisterWeaponEvents();
	});
	///////////////////////////
}

void Player::SetComponent(float meshSize, float mass)
{
	myMesh = AssetManager::Instance().guardMesh;
	// Collider
	SphereVolume* volume  = new SphereVolume(1);
	SetBoundingVolume((CollisionVolume*)volume);

	// Transform
	Transform& objectTransform = GetTransform();
	GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize));

	// Physics
	SetPhysicsObject(new PhysicsObject(&objectTransform, GetBoundingVolume()));
	GetPhysicsObject()->SetInverseMass(mass);
	GetPhysicsObject()->InitSphereInertia();
	
	// Render
	SetRenderObject(new RenderObject(
		&objectTransform,
		myMesh,
		AssetManager::Instance().playerTex,
		AssetManager::Instance().characterShader,
		RenderObjectType::Skinned)
	);
	
	// Manually set the rendering offset for the model,
	// which will be used in constructing the modelMatrix in GameTecRender
	// during camera and shadow rendering.
	renderObject->renderOffset = Vector3(0, -0.5, 0);
	
	animator = new Animator(renderObject);
	animator->LoadAnimation(AnimationType::Player_Walk);
	animator->LoadAnimation(AnimationType::Player_Idle);
	animator->LoadAnimation(AnimationType::Player_Jump);
	animator->Play(AnimationType::Player_Idle, true);
}

/// Create a player instance in world
/// @param world the scene
/// @param camera his camera
/// @param position instantiate position
/// @return 
Player* Player::Instantiate(GameWorld* world, ThirdPersonCamera* camera, const Vector3& position)
{
	Player* player = new Player();

	// Set its location and rotation
	player->GetTransform().SetPosition(position);
	player->GetTransform().SetOrientation(Quaternion());
	player->SetComponent(2, 5);
	player->playerObject = player;
	player->myWorld = world;
	player->Init(camera);

	// Add to the GameWorld
	if (world) {
		world->AddGameObject(player);
	}
	camera->SetFollowObject(player);
	
	//player-> animator->Play("Role_Walk",true,1);
	
	return player;
}
void Player::PausedUpdate(float dt) {

	DisplayUI();
	HealthCheck();
	animator->Update(dt);


}

void Player::Update(float dt) {

	UpdateGroundStatus();
	FixBounce();
	
	HandleDash(dt); 
	HandleAim();
	HandleSwitchWeapon();

	bool firing = Window::GetMouse()->ButtonDown(MouseButtons::Left);
	if (firing) {
		if (isDashing) {
			isDashing   = false;
			dashTimer   = 0.0f;
			renderObject->SetColour(defaultColour);
		}
		if (playerPhysicObject) {
			Vector3 currentVel = playerPhysicObject->GetLinearVelocity();
			playerPhysicObject->SetLinearVelocity(Vector3(0, currentVel.y, 0));
		}
		FaceAimDirection(dt);
	}
	else {
		if (!isDashing && !isDead) {
			HandleMovement(dt, inputDir);
		}
		HandleRotation(dt);
	}

	HandleJump(dt);
	DisplayUI();
	HealthCheck();
	animator->Update(dt);
	currentWeapon->Update(dt, firing, aimDir);

	if (isTemporaryColourActive) {
		colourTimer -= dt;
		if (colourTimer <= 0.0f) {
			this->GetRenderObject()->SetColour(defaultColour);
			isTemporaryColourActive = false;
		}
	}
	if (isTempTimerActive) {
		Debug::Print(showItem, Vector2(40, 40));
		msgTimer -= dt;
		if (msgTimer <= 0.0f) {
			isTempTimerActive = false;
		}
	}
}


void Player::FaceAimDirection(float dt) {
	
	Vector3 horizontalAim = Vector3(aimDir.x, 0.0f, aimDir.z);
	if (Vector::Length(horizontalAim) < 0.01f)
		return;  // make sure valid
	
	horizontalAim = Vector::Normalise(horizontalAim);
	float yawRadians = std::atan2(horizontalAim.x, horizontalAim.z);
	float yawDegrees = Maths::RadiansToDegrees(yawRadians);

	
	Quaternion desiredOrientation = Quaternion::EulerAnglesToQuaternion(0.0f, yawDegrees + 180.0f, 0.0f);
	Quaternion currentOrientation = GetTransform().GetOrientation();

	float dot = Quaternion::Dot(currentOrientation, desiredOrientation);
	if (dot < 0.0f) {
		desiredOrientation = -desiredOrientation;
	}

	// smooth rotation
	float rotateSpeed = 40.0f;
	float t = rotateSpeed * dt;
	Quaternion finalOrientation = Quaternion::Slerp(currentOrientation, desiredOrientation, t);
	GetTransform().SetOrientation(finalOrientation);
	animator->Play(AnimationType::Player_Idle,true);
}

void Player::HealthCheck()
{
	if (health <= 0)
	{
		Debug::Print("Dead", Vector2(40,40),Vector4(1,0,0,1));

		if (!isDead) { 
			isDead = true;
			AudioManager::GetInstance().PlayEvent("event:/Player Dead");
		}

	}

}

void Player::HandleInput()
{
	// each frame clear the input buffer
	inputDir = Vector2(0, 0);

	// Check if any movement key is pressed
	bool isMoving = false;

	// detect keyboard input
	if (Window::GetKeyboard()->KeyDown(KeyCodes::W)) {
		inputDir.y += -1.0f;  // front
		isMoving = true;
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::S)) {
		inputDir.y += 1.0f;   // back
		isMoving = true;
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::A)) {
		inputDir.x += -1.0f;  // left
		isMoving = true;
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::D)) {
		inputDir.x += 1.0f;   // right
		isMoving = true;
	}

	if (isMoving) {
		bool isPlaying = false;
		if (footstepEvent) {
			FMOD_STUDIO_PLAYBACK_STATE state;
			footstepEvent->getPlaybackState(&state);
			isPlaying = (state == FMOD_STUDIO_PLAYBACK_PLAYING);
		}

		if (!isPlaying) {
			footstepEvent = AudioManager::GetInstance().PlayEvent("event:/Player Running");
		}
		else {
			footstepEvent->setPaused(false);
		}
	}
	else {
		// stop footstep sound
		if (footstepEvent) {
			footstepEvent->setPaused(true);
		}
	}
}

void Player::HandleMovement(float dt, Vector2 inputDir) {
	if (!playerPhysicObject) return;
	HandleInput();
	
	Vector3 levelCamFront = Vector3(myCam->front.x, 0, myCam->front.z);
	Vector3 levelCamRight = Vector3(myCam->right.x, 0, myCam->right.z);
	
	moveDir = Vector::Normalise(-levelCamFront * inputDir.y + levelCamRight * inputDir.x);
	Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + moveDir);
	
	// if there has input value, add force
	if (Vector::Length(moveDir) > 0) {
		Vector3 force = Vector::Normalise(moveDir) * acceleratForce;
		playerPhysicObject->AddForce(force);  // add force to object
		animator->Play(AnimationType::Player_Walk, true);
	}
	else
	{
		animator->Play(AnimationType::Player_Idle, true);
	}
	ClampSpeed(dt);
}

// limit speed 
void Player::ClampSpeed(float dt) {
	if (!playerPhysicObject) return;

	Vector3 velocity = playerPhysicObject->GetLinearVelocity();
	float speed = Vector::Length(Vector3(velocity.x, 0, velocity.z));

	if (speed > maxSpeed) {
		Vector3 horizontalVelocity = Vector3(velocity.x, 0, velocity.z);
		Vector3 deceleration = -Vector::Normalise(horizontalVelocity) * decelerationFactor;

		if (Vector::Length(Vector3(playerPhysicObject->GetLinearVelocity().x, 0, playerPhysicObject->GetLinearVelocity().z)) > maxSpeed) {
			playerPhysicObject->AddForce(deceleration);
		}
		else {
			Vector3 limitedVelocity = Vector::Normalise(horizontalVelocity) * maxSpeed;
			playerPhysicObject->SetLinearVelocity(Vector3(limitedVelocity.x, velocity.y, limitedVelocity.z));
		}
	}
    
	if (Vector::Length(inputDir) < 0.1f) {
		playerPhysicObject->SetLinearVelocity(Vector3(0, velocity.y, 0));
	}
}


void Player::HandleRotation(float dt) {
	
	Vector3 horizontalDir = Vector3(moveDir.x, 0.0f, moveDir.z);
	float length = Vector::Length(horizontalDir);

	// if no input direction don't rotate 
	if (length < 0.01f) {
		return;
	}
	horizontalDir = Vector::Normalise(horizontalDir);

	// calculate rotation degrees
	float yawRadians = std::atan2(horizontalDir.x, horizontalDir.z);
	float yawDegrees = Maths::RadiansToDegrees(yawRadians);
	
	Quaternion desiredOrientation = Quaternion::EulerAnglesToQuaternion(0.0f, yawDegrees + 180.0f, 0.0f);
	Quaternion currentOrientation = playerObject->GetTransform().GetOrientation();

	float dot = Quaternion::Dot(currentOrientation, desiredOrientation);
	if (dot < 0.0f) {
		desiredOrientation = -desiredOrientation;
	}

	// rotate speed
	float rotateSpeed = 15.0f;
	float t = rotateSpeed * dt;

	// smooth rotation
	Quaternion finalOrientation = Quaternion::Slerp(
		currentOrientation,
		desiredOrientation,
		t
	);
	
	playerObject->GetTransform().SetOrientation(finalOrientation);
	this->GetTransform().SetOrientation(finalOrientation);
}

void Player::HandleFire(float dt)
{
	if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::Left))
	{
		currentWeapon->Fire();
	}
}

void Player::HandleDash(float dt) {
	if (isDashing) {
		if (!isTemporaryColourActive)
		{
			renderObject->SetColour(dashColour);
		}
		dashTimer -= dt;  // countdown timer
		if (dashTimer <= 0.0f) {
			isDashing = false;  // Dash ends, restore normal control
			renderObject->SetColour(defaultColour);
		}
		return;  // Ignore other inputs during dashing
	}

	// if press Shift
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::SHIFT)) {
		Vector3 velocity = playerPhysicObject->GetLinearVelocity();
		if (Vector::Length(velocity) > 0) {  // If there is velocity
			Vector3 dashForce = Vector::Normalise(velocity) * acceleratForce * dashForceMultiplier;
			playerPhysicObject->AddForce(dashForce);  // Dash direction follows the current velocity
			isDashing = true;
			dashTimer = dashCooldown;  // Start cooldown timer

			AudioManager::GetInstance().PlayEvent("event:/Dash");   // Dashsound
		}
	}
}

void Player::HandleJump(float dt) {
	if (!playerPhysicObject)
		return;
	
	if (isOnGround && Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE)) {
		jumpTimeCounter = 0.1f;
		isOnGround = false;
		AudioManager::GetInstance().PlayEvent("event:/Player jump");
	}
	
	if (jumpTimeCounter > 0) {
		playerPhysicObject->AddForce(Vector3(0, jumpForce * dt, 0));
		jumpTimeCounter -= dt;
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
		Debug::DrawLine(shootPoint, hitPoint, Debug::RED);
	} else {
		hitPoint = ray.GetPosition() + ray.GetDirection() * 100.0f;
		Debug::DrawLine(shootPoint, hitPoint, Debug::YELLOW);
	}
	shootPoint = this->GetTransform().GetPosition() + Vector3(0, 1.5, 0); // bullet instantiate offset
	aimDir = hitPoint - shootPoint;
}

void Player::OnCollisionBegin(GameObject* otherObject)
{
	//collides ground
	if (otherObject->tag=="Ground")
	{
		isOnGround =true;
	}

	//collides Enemy
	/*
	if (otherObject->tag=="Enemy")
	{
		std::cout << otherObject->GetName() << std::endl;
		Debug::DrawLine(this->GetTransform().GetPosition(), otherObject->GetTransform().GetPosition());
		if (isDashing)
		{
			Enemy* enemy = dynamic_cast<Enemy*>(otherObject);
			score += 15;
			SetTemporaryColour(attackColour, 0.25f);
			enemy->Reset();
		}
		else
		{

			health -= damage;
            AudioManager::GetInstance().PlaySound("GetHurt.wav");
			Enemy* enemy = dynamic_cast<Enemy*>(otherObject);
			SetTemporaryColour(damageColour, 0.25f);
			enemy->Reset();

		}
	}
	*/
	//collides Coin
	if (otherObject->tag=="Coin")

	{
		score += 10;
		otherObject->SetActive(false);
		RemoveObject(otherObject);
		SetTemporaryColour(collerctCoinColour, 0.25f);
	}

	if (otherObject->tag == "Passive") {
		otherObject->SetActive(false);
		RemoveObject(otherObject);
		PassiveItem* passiveItem = dynamic_cast<PassiveItem*>(otherObject);
		if (passiveItem) {
			passiveItem->UpdateCall();
			showItem = passiveItem->ShowItem();
			score += 100;
			SetTemporaryColour(collerctCoinColour, 0.5f);
			ShowTimer(showItem, 1.0f);
		}
	}
}

void Player::ShowTimer(std::string item, float duration) {
	msgTimer = duration;
	isTempTimerActive = true;
}

void Player::OnCollisionEnd(GameObject* otherObject)
{
	//nothing for now
}

void Player::SetTemporaryColour(const Vector4& colour, float duration) {
	colourTimer = duration;                
	isTemporaryColourActive = true;        // activate timer
	this->GetRenderObject()->SetColour(colour);
}

void Player::DisplayUI()
{
	/*float velocity = Vector::Length(playerPhysicObject->GetLinearVelocity());
	Debug::Print("V:" + std::format("{:.1f}", velocity), Vector2(5, 10));
	Debug::Print("HP:" + std::to_string(health), Vector2(5, 15));
	Debug::Print("Score:" + std::to_string(score), Vector2(80, 10));*/
}

void Player::RemoveObject(GameObject* gameObject)
{
	gameObject->GetTransform().SetPosition(Vector3(-1000, -1000, -1000));
}

void Player::UpdateGroundStatus() {
	
	Vector3 origin = GetTransform().GetPosition() + Vector3(0, -0.5f, 0);
	Vector3 direction(0, -1, 0);
	float rayLength = 0.51f;
	Ray groundRay(origin, direction);
	RayCollision collisionInfo;
	bool hit = myWorld->Raycast(groundRay, collisionInfo, true);
	if (hit && collisionInfo.rayDistance <= rayLength) {
		Debug::DrawLine(transform.GetPosition(),origin+direction,Debug::GREEN);
		isOnGround = true;
	} else {
		Debug::DrawLine(transform.GetPosition(),origin+direction,Debug::YELLOW);
		isOnGround = false;
	}
}

void Player::FixBounce()
{
	if (isOnGround && !wasOnGround) {
		Vector3 currentVel = playerPhysicObject->GetLinearVelocity();
		//reset vertical velocity to avoid bounce
		playerPhysicObject->SetLinearVelocity(Vector3(currentVel.x, 0, currentVel.z));
	}
	wasOnGround = isOnGround;
}

void Player::HandleSwitchWeapon()
{
	if(Window::GetKeyboard()->KeyDown(KeyCodes::NUM1)&&weaponPack[0]&&currentWeapon!=weaponPack[0])
	{
		currentWeapon = weaponPack[0];
		renderObject->subTextures[renderObject->subTextures.size()-1] = nullptr;
		OnSwitchWeaponEvent.Invoke(this);
		AudioManager::GetInstance().PlayEvent("event:/SwitchWeapon");
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM2)&&weaponPack[1]&&currentWeapon!=weaponPack[1])
	{
		currentWeapon = weaponPack[1];
		renderObject->subTextures[renderObject->subTextures.size()-1] = AssetManager::Instance().woodTex;
		OnSwitchWeaponEvent.Invoke(this);
		AudioManager::GetInstance().PlayEvent("event:/SwitchWeapon");
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM3)&&weaponPack[2]&&currentWeapon!=weaponPack[2])
	{
		currentWeapon = weaponPack[2];
		renderObject->subTextures[renderObject->subTextures.size()-1] = AssetManager::Instance().metalTex;
		OnSwitchWeaponEvent.Invoke(this);
		AudioManager::GetInstance().PlayEvent("event:/SwitchWeapon");
	}
}

void Player::RegisterWeaponEvents()
{
	currentWeapon->OnFireEvent.AddListener([](Weapon* w)
	{
		Debug::Print(std::to_string(w->getAmmo()), Vector2(52,50),Debug::RED);
	});
	currentWeapon->OnReloadStartEvent.AddListener([](Weapon* w)
	{
		Debug::Print("Start Reload", Vector2(52,50),Debug::RED);
	});
	currentWeapon->OnReloadEndEvent.AddListener([](Weapon* w)
	{
	Debug::Print("Finish Reload", Vector2(52,50),Debug::RED);
	});
}

