#pragma once
#include "Weapon.h"
using namespace NCL;
using namespace CSC8503;

class Pistol : public Weapon
{
public:
    Pistol(ThirdPersonCamera* camera);
    virtual ~Pistol();

    void Fire() override;
    
private:
    ThirdPersonCamera* myCamera;
};
