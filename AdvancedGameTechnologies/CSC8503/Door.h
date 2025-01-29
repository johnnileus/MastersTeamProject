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
            bool isMoving;      // 是否正在移动
            float moveSpeed;    // 门的移动速度
            float minHeight;    // 门的最小高度
            bool isMovingUp;    // 是否正在向上移动
            float originalHeight; // 门的原始高度（最高点）
            
        };
    }
    
};
