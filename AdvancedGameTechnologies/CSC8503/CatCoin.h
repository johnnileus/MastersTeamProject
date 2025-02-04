#pragma once

#pragma once

#include "GameObject.h"
#include "Player.h"

#include "Window.h"
#include "TutorialGame.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TutorialGame.h"

namespace NCL
{
    namespace CSC8503
    {
        class CatCoin : public GameObject
        {
        public:
            CatCoin();
            ~CatCoin();

            void Init();

            GameObject* coinGameObject;

            static CatCoin* Instantiate(GameWorld* world, const Vector3& position);

        protected:
    
        };
    }
}
