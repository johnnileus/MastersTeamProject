#include "Pistol.h"

#include "Bullet.h"

Pistol::Pistol(Player* owner):Weapon(10,15,0.4,false)
{
    myCamera=owner->myCam;
    myWorld=owner->myWorld;
    this->owner=owner;
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

    //reduce ammo
    ammo--;
    std::cout << "Weapon fired! Damage: " << damage<< ", ammo remaining: " << ammo << std::endl;

    //creat a bullet
    Bullet::Instantiate(myWorld,owner->GetTransform().GetPosition(),Vector3(1,0,0),this);

    //camera effect
    myCamera->Shake(0.05,0.1f);
}

