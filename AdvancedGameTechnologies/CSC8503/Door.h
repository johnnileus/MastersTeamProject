#pragma once
#include "GameObject.h"
#include "GameWorld.h"

namespace NCL
{
    namespace CSC8503
    {
        class Door : public GameObject
        {
        public:
            Door();
            ~Door();

            void Init(GameObject* doorPanel);

            void Update(float dt);

            GameObject* doorObject;
            GameObject* CreateDoorPanelObject(GameWorld* world, const Vector3& position, const Vector3& size, const Quaternion& rotation);
            
            static Door* Instantiate(GameWorld* world,
                                     const Vector3& trigger_pos,
                                     const Vector3& panel_pos,
                                     const Quaternion& trigger_quaternion,
                                     const Quaternion& panel_quaternion );
            
            void UpdateDoorMovement(float dt);
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
