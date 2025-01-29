#include "Enemy.h"

#include "State.h"
#include "StateMachine.h"
#include "StateTransition.h"

using namespace NCL;
using namespace CSC8503;

Enemy::Enemy(Player* player, GameWorld* world)
{
    name = "enemy";
    tag = "Enemy";
    targetPlayer = player;
    stateMachine= new StateMachine();
    acceleratForce=5;
    maxSpeed = 10.3;
    decelerationForce =10;
    rotationFactor =50;
    defaultColour=Vector4(1,1,1,1);
    hitColour = Vector4(1,0,0,1);
    myWorld=world;
}

void Enemy::Init() {
    // Idle state: Does nothing
    State* idleState = new State([&](float dt) {
        this->IdleState(dt);
    });

    // Chase state: Moves toward the player
    State* chaseState = new State([&](float dt) {
        this->ChaseState(dt);
    });

    // Add states to the state machine
    stateMachine->AddState(idleState);
    stateMachine->AddState(chaseState);

    // Transition from idle to chase when the player is within a certain range
    stateMachine->AddTransition(new StateTransition (idleState, chaseState, [&]() -> bool {
        Vector3 enemyPos = this->GetTransform().GetPosition();
        Vector3 playerPos = targetPlayer->GetTransform().GetPosition();
        return Vector::Length (enemyPos - playerPos) < 20.0f; // Chase when within 20 units
    }));

    // Transition from chase to idle when the player is out of range
    stateMachine->AddTransition(new StateTransition(chaseState, idleState, [&]() -> bool {
        Vector3 enemyPos = this->GetTransform().GetPosition();
        Vector3 playerPos = targetPlayer->GetTransform().GetPosition();
        return Vector::Length (enemyPos - playerPos) >= 20.0f; // Idle when beyond 20 units
    }));
}

void Enemy:: Update(float dt)
{
    stateMachine->Update(dt);
    ClampSpeed(dt);
    HandleRotation(dt);
}



void Enemy::IdleState(float dt) {
    FollowPath(dt);
}

void Enemy::FollowPath(float dt) {
    if (movePath.empty()) {
        return; 
    }

    if (currentNodeIndex >= movePath.size()) {
        currentNodeIndex = 0; // 重置为第一个节点，形成循环
    }

    Vector3 currentPos = this->GetTransform().GetPosition();
    Vector3 targetPos = movePath[currentNodeIndex];
    Vector3 direction = Vector::Normalise(targetPos - currentPos);

    // 移动敌人向当前目标节点
    this->GetPhysicsObject()->AddForce(direction * acceleratForce);

    // 检查是否到达当前目标节点
    if (Vector::Length(targetPos - currentPos) < waypointThreshold) {
        currentNodeIndex++; // 移动到下一个节点
    }
}



void Enemy::ChaseState(float dt) {
    if (!targetPlayer) return;
    //Debug::DrawLine(gameObject->GetTransform().GetPosition(),targetPlayer->GetTransform().GetPosition());
    // Move toward the player
    Vector3 direction =Vector::Normalise(targetPlayer->GetTransform().GetPosition() - this->GetTransform().GetPosition());
    this->GetPhysicsObject()->AddForce(direction * acceleratForce); // Apply force toward player
}

void Enemy::ClampSpeed(float dt) {
    if (!this->GetPhysicsObject()) return;

    Vector3 velocity = this->GetPhysicsObject()->GetLinearVelocity();
    float speed = Vector::Length(velocity);
    if (speed > maxSpeed) {
        // 计算减速方向，与速度方向相反
        Vector3 deceleration = -Vector::Normalise(velocity) * decelerationForce ;

        // 如果减速后速度仍超过最大速度，继续施加减速力
        if (speed > maxSpeed) {
            this->GetPhysicsObject()->AddForce(deceleration);
        } else {
            // 速度接近最大值时直接设置速度为最大速度
            Vector3 limitedVelocity = Vector::Normalise(velocity)  * maxSpeed;
            this->GetPhysicsObject()->SetLinearVelocity(limitedVelocity);
        }
    }
}

void Enemy::HandleRotation(float dt) {
    if (!this->GetPhysicsObject()) return;

    // 获取当前线性速度
    Vector3 velocity = this->GetPhysicsObject()->GetLinearVelocity();

    // 如果速度非常低，不进行旋转
    if (Vector::Length(velocity) < 0.01f) return;

    // 计算旋转轴：速度方向的垂直方向（右手规则）
     Vector3 rotationAxis = Vector::Normalise(Vector3(velocity.z, 0, -velocity.x));

    // 计算角速度：线速度大小除以半径
    float angularSpeed = Vector::Length(velocity) / this->GetTransform().GetScale().x * rotationFactor;  // 半径等于缩放比例

    // 计算旋转角度
    float rotationAngle = angularSpeed * dt;

    // 创建四元数表示旋转
    Quaternion rotation = Quaternion::AxisAngleToQuaterion(rotationAxis, rotationAngle);

    // 应用旋转到敌人的变换
    this->GetTransform().SetOrientation(rotation * this->GetTransform().GetOrientation());
}



void Enemy::SetMovePath(const std::vector<Vector3>& path) {
    movePath = path;
    currentNodeIndex = 0; // 从第一个节点开始移动
}


void Enemy::Reset() {
    if (movePath.empty()) {
        std::cout << "No path available for reset!" << std::endl;
        this->GetTransform().SetPosition(Vector3(0,0,0));
        return; 
    }
    
    this->GetTransform().SetPosition(movePath[0]);

    // 将物理状态重置
    this->GetPhysicsObject()->ClearForces();               // 清除所有作用力
    this->GetPhysicsObject()->SetLinearVelocity(Vector3()); // 清除线性速度

    // 重置当前路径索引
    currentNodeIndex = 0;

    std::cout << "Enemy reset to path start: " 
              << movePath[0].x << ", " 
              << movePath[0].y << ", " 
              << movePath[0].z << std::endl;
}



Enemy::~Enemy()
{

}
