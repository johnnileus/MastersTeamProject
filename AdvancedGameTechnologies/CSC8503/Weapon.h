#pragma once
#include <iostream>

#include "ThirdPersonCamera.h"

namespace NCL {
    namespace CSC8503 {

        class Weapon {
        protected:
            int   ammo;           // current ammo
            int   maxAmmo;        // max ammo per magazine
            int   damage;         // damage per bullet

            float shotInterval;   // time between shots for auto-fire
            bool  canAutoFire;    // if allowed auto-fire 

            float shotTimer;      // countdown timer until the next shot is allowed
            bool  wasFiringLastFrame;
        

        public:
            Weapon(int maxAmmo, int damage, float interval, bool autoFire)
                : ammo(maxAmmo)
                , maxAmmo(maxAmmo)
                , damage(damage)
                , shotInterval(interval)
                , canAutoFire(autoFire)
                , shotTimer(0.0f)
                , wasFiringLastFrame(false)
            {
                
            }

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
