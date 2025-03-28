#pragma once
#include "Animator.h"
#include "Weapon.h"
#include "AssetManager.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "Player.h"

namespace NCL
{
    namespace CSC8503
    {
        class Bullet : GameObject
        {
        public:
            Bullet(float damage, float speed, float distance);
            ~Bullet();

            void Update(float dt);

            bool IsDead() const { return isDead; }
            
            static Bullet* Instantiate(GameWorld* world, const Vector3& position, const Vector3& direction, Weapon* weapon, float distance = 100,Player* player = nullptr);
            GameWorld* myWorld;
        private:
            Vector3 direction; //move direction
            float speed;
            float damage;
            float distance;
            float travelled;
            bool isDead;

            void SetComponent(float meshSize,float inverseMass);

            void OnCollisionBegin(GameObject* otherObject) override;
        };
    }
}

