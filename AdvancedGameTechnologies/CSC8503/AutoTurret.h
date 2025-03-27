#pragma once
#include "GameWorld.h"
#include "Weapon.h"

namespace NCL
{
    namespace CSC8503
    {
        class AutoTurret
        {
        public:
            AutoTurret();
            ~AutoTurret();
            static AutoTurret* Instantiate(GameWorld* world, const Vector3& position);

        private:
            void Init();
            void SetComponent(float meshSize, float inverseMass);

            float damage;
        };
    }
}
