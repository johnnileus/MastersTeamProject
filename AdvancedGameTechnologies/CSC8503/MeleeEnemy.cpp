#include "MeleeEnemy.h"void MeleeEnemy::RestState(float dt) {
void MeleeEnemy::RestState(float dt) {
    this->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
    float healthIncrease = maxHealth * 0.01f * dt;
}
