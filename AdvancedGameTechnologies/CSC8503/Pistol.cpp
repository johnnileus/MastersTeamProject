#include "Pistol.h"
#include "Player.h"
#include "Bullet.h"

#include "AudioManager.h"

Pistol::Pistol(Player* owner):Weapon(10,10,0.4,false,2)
{
    myCamera = owner->myCam;
    myWorld = owner->myWorld;
    this->owner = owner;
    damage = owner->GetDamage() + Weapon::getDamage();
    weaponType = WeaponType::Pistol;

    reloadSoundEvent = "event:/Pistol reload";
}

Pistol::~Pistol()
{

}

void Pistol::Fire()
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
    AudioManager::GetInstance().PlayEvent("event:/Pistol");
}
