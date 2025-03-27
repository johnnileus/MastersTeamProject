#include "GhostEnemy.h"
#include "Vector.h"
#include "PhysicsObject.h"
#include "State.h"
#include "StateTransition.h"

using namespace NCL;
using namespace CSC8503;

void  GhostEnemy::InitStateMachine() {
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
        return currentTarget && (Vector::Length(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) < 60.0f;
        }));

    stateMachine->AddTransition(new StateTransition(chaseState, patrolState, [&]() -> bool {
        return !currentTarget || (Vector::Length(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) >= 60.0f;
        }));

    stateMachine->AddTransition(new StateTransition(chaseState, attackState, [&]() -> bool {
        if (currentTarget && (Vector::Length(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) < 15.0f) {
            this->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
            return true;
        }
        return false;
        }));

    stateMachine->AddTransition(new StateTransition(attackState, chaseState, [&]() -> bool {
        return currentTarget && (Vector::Length(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) >= 15.0f;
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
        return this->currentHealth < 0.15f * this->maxHealth && (Vector::Length(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) > 80.0f;
        }));

    stateMachine->AddTransition(new StateTransition(restState, patrolState, [&]() -> bool {
        return this->currentHealth > 0.5f * this->maxHealth;
        }));

    stateMachine->AddTransition(new StateTransition(restState, retreatState, [&]() -> bool {
        return currentTarget && (Vector::Length(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) < 30.0f;
        }));
}
void GhostEnemy::PatrolState() {
    float closestDistance = std::numeric_limits<float>::max();
    NCL::Maths::Vector3 enemyPosition = this->GetTransform().GetPosition();

    for (Player* player : players) {
        float distance = Vector::Length(enemyPosition - player->GetTransform().GetPosition());
        if (distance < closestDistance) {
            closestDistance = distance;
            this->currentTarget = player;
        }
    }


    if (this->currentNode == this->destination || this->destination == nullptr || this->path.size() == 0) {
        SetDestination();
        setCurrentNode(this->GetCurrentPosition().x, this->GetCurrentPosition().z);
        FindPath();
    }
    FollowPath();
}

void GhostEnemy::ChaseState() {
    if (currentTarget) {
        NCL::Maths::Vector3 playerPosition = currentTarget->GetTransform().GetPosition();

        NavMeshNode* closestNode = nullptr;
        float closestDistance = std::numeric_limits<float>::max();
        for (NavMeshNode* node : nodeGrid->GetAllNodes()) {
            float distance = Vector::Length(playerPosition - node->GetPosition());
            if (distance < closestDistance) {
                closestDistance = distance;
                closestNode = node;
            }
        }

        if (closestNode && closestNode != this->destination || this->path.size() == 0) {
            this->destination = closestNode;
            FindPath();
        }
        if (this->destination != nullptr) {
            FollowPath();
        }
    }
}

void GhostEnemy::AttackState(float dt) {
    if (attackCooldown <= 0) {
        NCL::Maths::Vector3 enemyPosition = this->GetTransform().GetPosition();
        float maxDistance = 30.0f;

        for (Player* player : players) {
            NCL::Maths::Vector3 playerPosition = player->GetTransform().GetPosition();
            float distance = Vector::Length(enemyPosition - playerPosition);

            if (distance <= maxDistance) {
                float damageFactor = 1.0f - (distance / maxDistance);
                float damageToApply = this->damage * damageFactor;
                player->ApplyDamage(damageToApply);
            }
        }
        this->KillEnemy();
        this->attackCooldown = 8;
    }
    else {
        this->attackCooldown -= dt;
    }
}

void GhostEnemy::RetreatState() {
    NCL::Maths::Vector3 playerPosition = currentTarget->GetTransform().GetPosition();
    NCL::Maths::Vector3 enemyPosition = this->GetTransform().GetPosition();
    NCL::Maths::Vector3 direction = Vector::Normalise(enemyPosition - playerPosition);
    NCL::Maths::Vector3 retreatPosition = enemyPosition + (direction * 50.0f);

    NavMeshNode* retreatNode = nullptr;
    float closestDistance = std::numeric_limits<float>::max();
    for (NavMeshNode* node : nodeGrid->GetAllNodes()) {
        float distance = Vector::Length(node->GetPosition() - retreatPosition);
        if (distance < closestDistance) {
            closestDistance = distance;
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

void GhostEnemy::RestState(float dt) {
    this->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
    float healthIncrease = maxHealth * 0.01f * dt;
    this->currentHealth = std::min(this->currentHealth + healthIncrease, this->maxHealth);
}

void GhostEnemy::UpdateEnemy(float dt) {
    this->stateMachine->Update(dt);

    float cooldownPercentage = this->attackCooldown / 8;
    Vector4 whiteColor = Vector4(0.5f, 0, 0.5f, 0.5f);
    Vector4 yellowColor = Vector4(1, 1, 0, 0.5f);

    if (cooldownPercentage > 0.9f) {
        this->GetRenderObject()->SetColour(whiteColor);
    }
    else {
        float flashSpeed = 0.25f + (0.75f * (cooldownPercentage / 0.9f));
        float t = fmod(attackCooldown, flashSpeed) / flashSpeed;
        Vector4 currentColor = (t < 0.5f) ? whiteColor : yellowColor;
        this->GetRenderObject()->SetColour(currentColor);
    }
}
