#pragma once
#include "GameWorld.h"
#include "Weapon.h"
namespace NCL
{
    namespace CSC8503
    {
        class Player;
        class Shotgun : public Weapon
        {
        public:
            Shotgun(Player* owner);
            virtual ~Shotgun();

            void Fire() override;

        private:
            ThirdPersonCamera* myCamera;
            GameWorld* myWorld;
            Player* owner;

            float RandomFloat(float min, float max);
            Vector3 RandomDirectionInCone(const Vector3 &centralDir, float halfAngle);
        };
    }
};

