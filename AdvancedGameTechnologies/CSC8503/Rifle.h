#pragma once
#include "GameWorld.h"
#include "Player.h"
#include "Weapon.h"
namespace NCL
{
    namespace CSC8503
    {
        class Rifle : public Weapon
        {
        public:
            Rifle(Player* owner);
            virtual ~Rifle();

            void Fire() override;

        private:
            ThirdPersonCamera* myCamera;
            GameWorld* myWorld;
            Player* owner;
        };
    }
};

