#include "Enemy.h"

#include "AssetManager.h"
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
    maxSpeed = 10.3f;
    decelerationForce =10;
    rotationFactor =50;
    defaultColour=Vector4(1,1,1,1);
    hitColour = Vector4(1,0,0,1);
    myWorld=world;
    mass=10.0f;
    size=1;

    SetComponent(size, mass);
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
    //stateMachine->Update(dt);
    ClampSpeed(dt);
    HandleRotation(dt);
}

void Enemy::SetComponent(float meshSize,float inverseMass)
{
    //Collider
    SphereVolume* volume  = new SphereVolume(meshSize);
    SetBoundingVolume((CollisionVolume*)volume);

    //Transform
    GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize));

    //Render
    SetRenderObject(new RenderObject(
        &GetTransform(),
        AssetManager::Instance().sphereMesh,
        AssetManager::Instance().woodTex,
        AssetManager::Instance().basicShader));

    //Physics
    SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));
    GetPhysicsObject()->SetInverseMass(inverseMass);
    GetPhysicsObject()->InitSphereInertia();
}


void Enemy::IdleState(float dt) {
    FollowPath(dt);
}

void Enemy::FollowPath(float dt) {
    if (movePath.empty()) {
        return; 
    }

    if (currentNodeIndex >= movePath.size()) {
        currentNodeIndex = 0; // Reset to the first node to form a loop.
    }

    Vector3 currentPos = this->GetTransform().GetPosition();
    Vector3 targetPos = movePath[currentNodeIndex];
    Vector3 direction = Vector::Normalise(targetPos - currentPos);

    // Move the enemy to the current target node.
    this->GetPhysicsObject()->AddForce(direction * acceleratForce);

    // Check if the current target node has been reached.
    if (Vector::Length(targetPos - currentPos) < waypointThreshold) {
        currentNodeIndex++; // move to next node
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
        //caculate the deceleration direction
        Vector3 deceleration = -Vector::Normalise(velocity) * decelerationForce ;

        // If the speed remains above the maximum after deceleration, continue applying deceleration force
        if (speed > maxSpeed) {
            this->GetPhysicsObject()->AddForce(deceleration);
        } else {
            //If the speed is close to the maximum, set it directly to the maximum speed.
            Vector3 limitedVelocity = Vector::Normalise(velocity)  * maxSpeed;
            this->GetPhysicsObject()->SetLinearVelocity(limitedVelocity);
        }
    }
}

void Enemy::HandleRotation(float dt) {
    if (!this->GetPhysicsObject()) return;

    // Get the current linear velocity.
    Vector3 velocity = this->GetPhysicsObject()->GetLinearVelocity();
    
    if (Vector::Length(velocity) < 0.01f) return;

    // If the speed is very low, do not rotate.
     Vector3 rotationAxis = Vector::Normalise(Vector3(velocity.z, 0, -velocity.x));

    // Calculate angular velocity: linear speed divided by radius.
    float angularSpeed = Vector::Length(velocity) / this->GetTransform().GetScale().x * rotationFactor; 

    //Calculate the rotation angle.
    float rotationAngle = angularSpeed * dt;

    // Create a quaternion to represent the rotation.
    Quaternion rotation = Quaternion::AxisAngleToQuaterion(rotationAxis, rotationAngle);

    // Apply the rotation to the enemy's transform.
    this->GetTransform().SetOrientation(rotation * this->GetTransform().GetOrientation());
}

Enemy* Enemy::Instantiate(GameWorld* world, std::vector<Enemy*>& enemyList, Player* player, const Vector3& position)
{
    Enemy* enemy = new Enemy(player,world);

    // Set it's location and rotation
    enemy->GetTransform().SetPosition(position);
    enemy->GetTransform().SetOrientation(Quaternion());
    enemy->Init();
    enemyList.push_back(enemy);
    
    // Add to the GameWorld
    if (world) {
        world->AddGameObject(enemy);
    }

    return enemy;
}


void Enemy::SetMovePath(const std::vector<Vector3>& path) {
    movePath = path;
    currentNodeIndex = 0; // move from the first node
}


void Enemy::Reset() {
    if (movePath.empty()) {
        std::cout << "No path available for reset!" << std::endl;
        this->GetTransform().SetPosition(Vector3(0,0,0));
        return; 
    }
    
    this->GetTransform().SetPosition(movePath[0]);

    // reset the physic
    this->GetPhysicsObject()->ClearForces();               // clear force
    this->GetPhysicsObject()->SetLinearVelocity(Vector3()); // reset the speeds

    // reset the path finding index
    currentNodeIndex = 0;

    //std::cout << "Enemy reset to path start: " 
    //          << movePath[0].x << ", " 
    //          << movePath[0].y << ", " 
    //          << movePath[0].z << std::endl;
}

Enemy::~Enemy()
{
    delete boundingVolume;
    delete physicsObject;
    delete renderObject;
    delete networkObject;
}
