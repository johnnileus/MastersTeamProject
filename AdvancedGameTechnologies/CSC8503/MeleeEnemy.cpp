#include "MeleeEnemy.h"void MeleeEnemy::RestState(float dt) {
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
