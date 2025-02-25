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

            Vector3 front;
            Vector3 right;

            void Update(float dt);

            void Shake(float strength, float duration);

            void SetPitch(float p) { pitch = p; }
            void SetYaw(float y)   { yaw   = y; }

            float orbitRadius;

        protected:
            Camera* camera;
            KeyboardMouseController& inputController;
            GameObject* followObject;

            void UpdateShakeTimer(float dt);

            Vector3 offset;       // Initial offset from target (e.g. (0,10,-20))
            float   pitch;        // Rotation around X (degrees)
            float   yaw;          // Rotation around Y (degrees)
            float   rotationSpeed;

            //Camera Effect
            float shakeTime; //shake remain time
            float shakeDuration;
            float shakeStrength;
        };
    }
}
