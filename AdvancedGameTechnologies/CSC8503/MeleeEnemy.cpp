#include "MeleeEnemy.h"void MeleeEnemy::RestState(float dt) {
void  MeleeEnemy::InitStateMachine() {
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
        return currentTarget && (Vector::Length(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) < 20.0f;
        }));

    stateMachine->AddTransition(new StateTransition(chaseState, patrolState, [&]() -> bool {
        return !currentTarget || (Vector::Length(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) >= 20.0f;
        }));

    stateMachine->AddTransition(new StateTransition(chaseState, attackState, [&]() -> bool {
        return currentTarget && (Vector::Length(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) < 5.0f;
        }));

    stateMachine->AddTransition(new StateTransition(attackState, chaseState, [&]() -> bool {
        return currentTarget && (Vector::Length(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) >= 5.0f;
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
        return this->currentHealth < 0.15f * this->maxHealth && (Vector::Length(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) > 50.0f;
        }));

    stateMachine->AddTransition(new StateTransition(restState, patrolState, [&]() -> bool {
        return this->currentHealth > 0.5f * this->maxHealth;
        }));

    stateMachine->AddTransition(new StateTransition(restState, retreatState, [&]() -> bool {
        return currentTarget && (Vector::Length(this->GetTransform().GetPosition() - currentTarget->GetTransform().GetPosition())) < 30.0f;
        }));
}
void MeleeEnemy::PatrolState() {
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
        FindPath();
    }
    FollowPath();
}

void MeleeEnemy::ChaseState() {
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

void MeleeEnemy::AttackState(float dt) {
    if (attackCooldown <= 0) {
        NCL::Maths::Vector3 direction = Vector::Normalise(currentTarget->GetTransform().GetPosition() - this->GetTransform().GetPosition());
        this->GetPhysicsObject()->AddForce(direction * chargeForce);
        attackCooldown = 3;
    }
    else {
        attackCooldown -= dt;
    }
}

void MeleeEnemy::RetreatState() {
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

void MeleeEnemy::RestState(float dt) {
    this->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
    float healthIncrease = maxHealth * 0.01f * dt;
}
