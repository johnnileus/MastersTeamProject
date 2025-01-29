#pragma once
#include "GameObject.h"

namespace NCL
{
    namespace CSC8503
    {
        class Door : public GameObject
        {
        public:
            Door();
            ~Door();

            void Init();

            void Update(float dt);

            GameObject* doorObject;

            void OnCollisionBegin(GameObject* otherObject) override;
        
        protected:
            bool isMoving;      
            float moveSpeed;    
            float minHeight;    // The door's minimum height.
            bool isMovingUp;    
            float originalHeight; // Door's original height (also is the maximum height)
            
        };
    }
    
};
