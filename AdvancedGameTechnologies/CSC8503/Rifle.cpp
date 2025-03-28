#include "Rifle.h"
#include "Player.h"
#include "AudioManager.h"
#include "Bullet.h"

Rifle::Rifle(Player* owner):Weapon(30,2,0.1f,true,3)
{
    myCamera = owner->myCam;
    myWorld = owner->myWorld;
    this->owner = owner;
    damage = owner->GetDamage() + Weapon::getDamage();
    weaponType = WeaponType::Rifle;

    reloadSoundEvent = "event:/Rifle reload";
}

Rifle::~Rifle()
{
    
}

void Rifle::Fire()
{
    if (ammo <= 0) {
        std::cout << "No ammo left, cannot fire!" << std::endl;
        Reload();
        return;
    }

    //reduce ammo
    ammo--;
    OnFireEvent.Invoke(this);
    std::cout << "Weapon fired! Damage: " << damage << ", ammo remaining: " << ammo << std::endl;

    //creat a bullet
    Bullet::Instantiate(myWorld,owner->shootPoint,aimDir,this,100,owner);

    //camera effect
    myCamera->Shake(0.07,0.1f);
    AudioManager::GetInstance().PlayEvent("event:/RifleFire");
}