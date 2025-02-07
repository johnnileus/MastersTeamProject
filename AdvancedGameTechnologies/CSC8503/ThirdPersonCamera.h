#pragma once

#include "../NCLCoreClasses/KeyboardMouseController.h"
#include "Camera.h"
#include "GameObject.h"
#include "Maths.h"
#include "Quaternion.h" // 需要有四元数类的头文件

namespace NCL {
    namespace CSC8503 {
        class ThirdPersonCamera {
        public:
            ThirdPersonCamera(Camera* cam, KeyboardMouseController& controller);
            ~ThirdPersonCamera();

            void SetFollowObject(GameObject* obj);
            void SetOffset(const Vector3& offset);

            void Update(float dt);

            void SetPitch(float p) { pitch = p; }
            void SetYaw(float y)   { yaw   = y; }

        protected:
            Camera* camera;
            KeyboardMouseController& inputController;
            GameObject* followObject;

            Vector3 offset;       // Initial offset from target (e.g. (0,10,-20))
            float   pitch;        // Rotation around X (degrees)
            float   yaw;          // Rotation around Y (degrees)
            float   rotationSpeed;
        };
    }
}
