#pragma once
#include "Animator.h"

using namespace NCL;
using namespace CSC8503;

class Bullet : GameObject
{
public:
    Bullet();
    ~Bullet();
    static Bullet* Instantiate(GameWorld* world, const Vector3& position, const Vector3& direction);
private:
    
};
