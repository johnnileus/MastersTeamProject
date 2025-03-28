#include "Shotgun.h"
#include "Player.h"
#include "AudioManager.h"
#include "Bullet.h"

Shotgun::Shotgun(Player* owner) 
    : Weapon(8, 5, 0.5f, true, 3) {
    myCamera = owner->myCam;
    myWorld  = owner->myWorld;
    this->owner = owner;
    damage = owner->GetDamage() + Weapon::getDamage();
    weaponType=WeaponType::Shotgun;

    reloadSoundEvent = "event:/Shotgun reload";
}

Shotgun::~Shotgun() {
    
}

void Shotgun::Fire() {
    if (ammo <= 0) {
        std::cout << "No ammo left, cannot fire!" << std::endl;
        Reload();
        return;
    }
    
    Vector3 dir = (Vector::Length(aimDir) < 0.001f) ? Vector3(0, 0, 1) : Vector::Normalise(aimDir);
    
    ammo--;
    OnFireEvent.Invoke(this);
    std::cout << "Weapon fired! Damage: " << damage 
              << ", ammo remaining: " << ammo << std::endl;
    
    float halfAngle = 5.0f * (3.14159265f / 180.0f);
    
    for (int i = 0; i < 8; ++i) {
        Vector3 bulletDir = RandomDirectionInCone(dir, halfAngle);
        Bullet::Instantiate(myWorld, owner->shootPoint, bulletDir, this, 20.0f,owner);
    }
    
    myCamera->Shake(0.12f, 0.15f);
    AudioManager::GetInstance().PlayEvent("event:/Shotgun");
}

float Shotgun::RandomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max - min)));
}

Vector3 Shotgun::RandomDirectionInCone(const Vector3 &centralDir, float halfAngle) {
    Vector3 normCentral = (Vector::Length(centralDir) < 0.001f) ? Vector3(0, 0, 1) : Vector::Normalise(centralDir);
    
    float cosTheta = RandomFloat(std::cos(halfAngle), 1.0f);
    float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
    float phi      = RandomFloat(0.0f, 2.0f * 3.14159265f);
    
    float x = sinTheta * std::cos(phi);
    float y = sinTheta * std::sin(phi);
    float z = cosTheta;
    Vector3 localDir(x, y, z);


    Vector3 zAxis(0.0f, 0.0f, 1.0f);
    if (std::fabs(Vector::Dot(normCentral, zAxis) - 1.0f) < 0.001f) {
        return Vector::Normalise(localDir);
    }
    
    Vector3 rotationAxis = Vector::Cross(zAxis, normCentral);
    float axisLength = Vector::Length(rotationAxis);
    if (axisLength < 0.001f) {
        return Vector::Normalise(localDir);
    }
    rotationAxis = rotationAxis / axisLength;
    float angle = std::acos(Vector::Dot(zAxis, normCentral));
    
    Vector3 rotatedDir = localDir * std::cos(angle)
                         + Vector::Cross(rotationAxis, localDir) * std::sin(angle)
                         + rotationAxis * Vector::Dot(rotationAxis, localDir) * (1 - std::cos(angle));
    return Vector::Normalise(rotatedDir);
}
