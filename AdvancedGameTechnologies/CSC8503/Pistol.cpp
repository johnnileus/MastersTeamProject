#include "Pistol.h"

Pistol::Pistol(ThirdPersonCamera* camera) : Weapon(10,15,0.5f,false)
{
    myCamera =camera;
}

Pistol::~Pistol()
{
    
}

void Pistol::Fire()
{
    if (ammo <= 0) {
        std::cout << "No ammo left, cannot fire!" << std::endl;
        return;
    }
    ammo--;
    std::cout << "Weapon fired! Damage: " << damage
              << ", ammo remaining: " << ammo << std::endl;
    myCamera->Shake(0.05,0.1f);
}

