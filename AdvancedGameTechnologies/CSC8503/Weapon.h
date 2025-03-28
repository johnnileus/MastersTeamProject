#pragma once
#include <Event.h>
#include <iostream>

#include "Enums.h"
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

            float reloadTimer;
            float reloadTime;
            bool isReloading;

        public:
            Weapon(int maxAmmo, int damage, float interval, bool autoFire, float reloadTime)
                : ammo(maxAmmo)
                , maxAmmo(maxAmmo)
                , damage(damage)
                , shotInterval(interval)
                , canAutoFire(autoFire)
                , shotTimer(0.0f)
                ,reloadTime(reloadTime)
                , wasFiringLastFrame(false)
            {
                reloadTimer = reloadTime;
            }
            Enums::WeaponType weaponType;

            virtual ~Weapon() {}

            virtual void Fire();
            virtual void Reload();

            // Update method, called each frame
            virtual void Update(float deltaTime, bool isFiring, const Vector3& direction);

            Vector3 aimDir;

            int  getAmmo()    const { return ammo; }
            int  getMaxAmmo() const { return maxAmmo; }
            int  getDamage()  const { return damage; }
            void setDamage(const int d) { damage = d; }
            bool IsAutoFire() const { return canAutoFire; }
            
            Enums::WeaponType GetWeaponType()
            {
                return weaponType;
            }

            Event<Weapon*> OnFireEvent;
            Event<Weapon*> OnReloadStartEvent;
            Event<Weapon*> OnReloadEndEvent;
        };

    }
}