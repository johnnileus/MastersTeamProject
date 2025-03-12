#pragma once
#include "GameWorld.h"
#include "Player.h"
#include "Weapon.h"
using namespace NCL;
using namespace CSC8503;

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