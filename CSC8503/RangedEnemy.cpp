#include "RangedEnemy.h"
#include "Vector.h"
#include "PhysicsObject.h"
#include "State.h"
#include "StateTransition.h"
#include "Ray.h"
#include "CollisionDetection.h"

using namespace NCL;
using namespace CSC8503;

void RangedEnemy::InitStateMachine() {
    State* patrolState = new State([&](float dt) -> void {
        this->PatrolState();
        });

    State* chaseState = new State([&](float dt) -> void {
        this->ChaseState();
        });

    State* attackState = new State([&](float dt) {
        this->AttackState(dt);
        });

    State* retreatState = new State([&](float dt) -> void {
        this->RetreatState();
        });

    State* restState = new State([&](float dt) {
        this->RestState(dt);
        });

    stateMachine->AddState(patrolState);
    stateMachine->AddState(chaseState);
    stateMachine->AddState(attackState);
    stateMachine->AddState(retreatState);
    stateMachine->AddState(restState);

    stateMachine->AddTransition(new StateTransition(patrolState, chaseState, [&]() -> bool {
        return currentTarget && (Vector::LengthSquared(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) < 10000.0f;
        }));

    stateMachine->AddTransition(new StateTransition(chaseState, patrolState, [&]() -> bool {
        return !currentTarget || (Vector::LengthSquared(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) >= 10000.0f;
        }));

    stateMachine->AddTransition(new StateTransition(chaseState, attackState, [&]() -> bool {
        if (currentTarget && (Vector::LengthSquared(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) < 400.0f && canSeePlayer()) {
            this->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
            return true;
        }
        return false;
        }));

    stateMachine->AddTransition(new StateTransition(attackState, chaseState, [&]() -> bool {
        return !currentTarget || (Vector::LengthSquared(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) >= 400.0f || !canSeePlayer();
        }));

    stateMachine->AddTransition(new StateTransition(patrolState, retreatState, [&]() -> bool {
        return this->currentHealth < 0.15f * this->maxHealth;
        }));

    stateMachine->AddTransition(new StateTransition(chaseState, retreatState, [&]() -> bool {
        return this->currentHealth < 0.15f * this->maxHealth;
        }));

    stateMachine->AddTransition(new StateTransition(attackState, retreatState, [&]() -> bool {
        return this->currentHealth < 0.15f * this->maxHealth;
        }));

    stateMachine->AddTransition(new StateTransition(retreatState, restState, [&]() -> bool {
        return this->currentHealth < 0.15f * this->maxHealth && (Vector::LengthSquared(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) > 10000.0f;
        }));

    stateMachine->AddTransition(new StateTransition(restState, patrolState, [&]() -> bool {
        return this->currentHealth > 0.5f * this->maxHealth;
        }));

    stateMachine->AddTransition(new StateTransition(restState, retreatState, [&]() -> bool {
        return currentTarget && (Vector::LengthSquared(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) < 2500.0f;
        }));
}

void RangedEnemy::PatrolState() {
    float closestDistanceSquared = std::numeric_limits<float>::max();
    NCL::Maths::Vector3 enemyPosition = this->GetTransform().GetPosition();

    for (Player* player : players) {
        float distanceSquared = Vector::LengthSquared(enemyPosition - player->GetTransform().GetPosition());
        if (distanceSquared < closestDistanceSquared) {
            closestDistanceSquared = distanceSquared;
            this->currentTarget = player;
        }
    }

    if (this->currentNode == this->destination || this->destination == nullptr || this->path.empty()) {
        SetDestination();
        setCurrentNode(this->GetCurrentPosition().x, this->GetCurrentPosition().z);
        FindPath();
    }
    FollowPath();
}

void RangedEnemy::ChaseState() {
    if (currentTarget) {
        NCL::Maths::Vector3 playerPosition = currentTarget->GetTransform().GetPosition();

        NavMeshNode* closestNode = nullptr;
        float closestDistanceSquared = std::numeric_limits<float>::max();
        for (NavMeshNode* node : nodeGrid->GetAllNodes()) {
            float distanceSquared = Vector::LengthSquared(playerPosition - node->GetPosition());
            if (distanceSquared < closestDistanceSquared) {
                closestDistanceSquared = distanceSquared;
                closestNode = node;
            }
        }

        if (closestNode && (closestNode != this->destination || this->path.empty())) {
            this->destination = closestNode;
            FindPath();
        }
        if (this->destination != nullptr) {
            FollowPath();
        }
    }
}

void RangedEnemy::AttackState(float dt) {
    if (attackCooldown <= 0) {
        currentTarget->ApplyDamage(damage);
        attackCooldown = 6;
    }
    else {
        attackCooldown -= dt;
    }
}

void RangedEnemy::RetreatState() {
    NCL::Maths::Vector3 playerPosition = currentTarget->GetTransform().GetPosition();
    NCL::Maths::Vector3 enemyPosition = this->GetTransform().GetPosition();
    NCL::Maths::Vector3 direction = Vector::Normalise(enemyPosition - playerPosition);
    NCL::Maths::Vector3 retreatPosition = enemyPosition + (direction * 50.0f);

    NavMeshNode* retreatNode = nullptr;
    float closestDistanceSquared = std::numeric_limits<float>::max();
    for (NavMeshNode* node : nodeGrid->GetAllNodes()) {
        float distanceSquared = Vector::LengthSquared(node->GetPosition() - retreatPosition);
        if (distanceSquared < closestDistanceSquared) {
            closestDistanceSquared = distanceSquared;
            retreatNode = node;
        }
    }

    if (retreatNode && retreatNode != this->destination) {
        this->destination = retreatNode;
        FindPath();
    }
    if (this->destination != nullptr) {
        FollowPath();
    }
}

void RangedEnemy::RestState(float dt) {
    this->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
    float healthIncrease = maxHealth * 0.01f * dt;
    this->currentHealth = std::min(this->currentHealth + healthIncrease, this->maxHealth);
}

void RangedEnemy::UpdateEnemy(float dt) {
    this->stateMachine->Update(dt);

    Vector4 startColor = Vector4(0, 0, 1, 1);
    Vector4 endColor = Vector4(0, 1, 1, 1);
    float t = 1.0f - (attackCooldown / 3.0f);
    Vector4 currentColor = startColor * (1.0f - t) + endColor * t;
    this->GetRenderObject()->SetColour(currentColor);
}

bool RangedEnemy::canSeePlayer() {
    if (currentTarget == nullptr) {
        return false;
    }

    NCL::Maths::Vector3 enemyPosition = this->GetTransform().GetPosition();
    NCL::Maths::Vector3 playerPosition = currentTarget->GetTransform().GetPosition();
    NCL::Maths::Vector3 direction = Vector::Normalise(playerPosition - enemyPosition);

    Ray ray(enemyPosition, direction);
    RayCollision collision;

    if (this->world->Raycast(ray, collision, true, this)) {
        GameObject* hitObject = static_cast<GameObject*>(collision.node);
        return hitObject && hitObject->tag == "Player";
    }

    return false;
}