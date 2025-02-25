#pragma once
#include <iostream>

namespace NCL {
    namespace CSC8503 {

        class Weapon {
        protected:
            int   ammo;           // Current ammo
            int   maxAmmo;        // Max ammo per magazine
            int   damage;         // Damage per bullet

            float shotInterval;   // Time (in seconds) between shots for auto-fire
            bool  canAutoFire;    // Whether auto-fire is supported

            float shotTimer;      // Countdown timer until the next shot is allowed
            bool  wasFiringLastFrame; // Tracks whether the fire button was down last frame

        public:
            Weapon(int maxAmmo, int damage, float interval, bool autoFire)
                : ammo(maxAmmo)
                , maxAmmo(maxAmmo)
                , damage(damage)
                , shotInterval(interval)
                , canAutoFire(autoFire)
                , shotTimer(0.0f)
                , wasFiringLastFrame(false)
            {}

            virtual ~Weapon() {}

            virtual void Fire();
            virtual void Reload();

            // Update method, called each frame
            //  - deltaTime: time elapsed since last frame
            //  - isFiring: whether the fire button is currently down
            virtual void Update(float deltaTime, bool isFiring);

            int  getAmmo()    const { return ammo; }
            int  getMaxAmmo() const { return maxAmmo; }
            int  getDamage()  const { return damage; }
            bool IsAutoFire() const { return canAutoFire; }
        };

    }
}
