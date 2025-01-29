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
	HandleDash(dt);  // 冲刺逻辑优先
	if (!isDashing||!isDead) {
		HandleMovement(dt);  // 只有不在冲刺时才能正常移动
	}
	HandleRotation(dt);
	HandleJump();
	DisplayUI();
	HealthCheck();

	// colour change timer
	if (isTemporaryColourActive) {
		colourTimer -= dt;
		if (colourTimer <= 0.0f) {
			// 恢复默认颜色
			this->GetRenderObject()->SetColour(defaultColour);
			isTemporaryColourActive = false; // 取消激活临时颜色
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
		// 计算减速方向（与速度方向相反）
		Vector3 deceleration = -Vector::Normalise(velocity)* decelerationFactor ;

		// 如果减速后速度仍然超出 maxSpeed，继续施加减速力
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

	// 获取球的线性速度
	Vector3 velocity = playerPhysicObject->GetLinearVelocity();

	// 如果速度接近零，不旋转
	if (Vector::Length(velocity)<0) return;

	// 计算旋转轴：速度方向的垂直方向（右手规则）
	Vector3 rotationAxis = Vector::Normalise(Vector3(velocity.z, 0, -velocity.x));

	// 计算旋转角速度：速度大小除以球体半径
	float angularSpeed =Vector::Length(velocity) / playerObject->GetTransform().GetScale().x*rotationFactor;  // 半径为缩放比例

	// 计算旋转角度
	float rotationAngle = angularSpeed * dt;

	// 生成四元数表示旋转
	Quaternion rotation = Quaternion::AxisAngleToQuaterion(rotationAxis, rotationAngle);

	// 应用旋转到球体变换
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
			isDashing = false;  // 冲刺结束，恢复正常控制
			
			renderObject->SetColour(defaultColour);
		}
		return;  // 冲刺期间忽略其他输入
	}

	// if press Shift
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::SHIFT)) {
		Vector3 velocity = playerPhysicObject->GetLinearVelocity();
		if (Vector::Length(velocity) > 0) {  // 如果球体有速度
			Vector3 dashForce = Vector::Normalise(velocity) * acceleratForce * dashForceMultiplier;
			playerPhysicObject->AddForce(dashForce);  // 冲刺方向与当前速度一致
			isDashing = true;
			dashTimer = dashCooldown;  // 开始计时
		}
	}
}


void Player::HandleJump() {
	if (!playerPhysicObject || !isOnGround) return;  // 只能在地面时跳跃

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE)) {
		Vector3 jump = Vector3(0, jumpForce, 0);  // 向上施加力
		playerPhysicObject->AddForce(jump);
		isOnGround = false;  // 标记不在地面
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









