#pragma once
#include "GameWorld.h"
#include "Weapon.h"
#include "Player.h"
namespace NCL{
    namespace CSC8503
    {
        class  Player;
        
        class Pistol : public Weapon
        {
        public:
            Pistol(Player* owner);
            virtual ~Pistol();

            void Fire() override;

        private:
            ThirdPersonCamera* myCamera;
            GameWorld* myWorld;
            Player* owner;
        };
    };
};