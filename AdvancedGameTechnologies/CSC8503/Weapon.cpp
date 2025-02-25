#include "Weapon.h"

using namespace NCL;
using namespace CSC8503;

void Weapon::Fire() {
    if (ammo <= 0) {
        std::cout << "No ammo left, cannot fire!" << std::endl;
        return;
    }
    ammo--;
    std::cout << "Weapon fired! Damage: " << damage
              << ", ammo remaining: " << ammo << std::endl;
}

void Weapon::Reload() {
    ammo = maxAmmo;
    std::cout << "Reload complete! Ammo: " << ammo << std::endl;
}

void Weapon::Update(float deltaTime, bool isFiring) {
    // Decrease shotTimer by the elapsed time each frame
    if (shotTimer > 0.0f) {
        shotTimer -= deltaTime;
    }

    if (canAutoFire) {
        // AUTO-FIRE MODE:
        // 1) Fire immediately if shotTimer <= 0 (i.e., cooldown is over)
        // 2) Reset shotTimer to shotInterval
        if (isFiring) {
            if (shotTimer <= 0.0f) {
                Fire();
                shotTimer = shotInterval;
            }
        }
    }
    else {
        // SINGLE-FIRE MODE:
        // Fire only on the transition from "not firing last frame" to "firing this frame"
        if (isFiring && !wasFiringLastFrame) {
            Fire();
        }
    }

    // Update the firing state for the next frame
    wasFiringLastFrame = isFiring;
}
