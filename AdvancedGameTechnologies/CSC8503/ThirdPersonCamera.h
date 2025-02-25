#pragma once

#include "../NCLCoreClasses/KeyboardMouseController.h"
#include "Camera.h"
#include "GameObject.h"
#include "Maths.h"
#include "Quaternion.h" // 需要有四元数类的头文件
#ifdef USEAGC
#include <PS5Controller.h>
#include <PS5Window.h>
#endif // USEAGC



namespace NCL {
    namespace CSC8503 {
        class ThirdPersonCamera {
        public:
#ifdef USEAGC
            ThirdPersonCamera(Camera* cam, NCL::PS5::PS5Controller& controller);
#else
            ThirdPersonCamera(Camera* cam, KeyboardMouseController& controller);
#endif // _WIN32

            
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
#ifdef USEAGC
            NCL::PS5::PS5Controller& inputController;
#else
			KeyboardMouseController& inputController;
#endif // USEAGC
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
