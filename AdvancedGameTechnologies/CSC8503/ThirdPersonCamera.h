#pragma once

#include "../NCLCoreClasses/KeyboardMouseController.h"
#include "Camera.h"
#include "GameObject.h"
#include "Maths.h" // For degrees/radians conversion if needed
#include <cmath>   // For sin, cos

namespace NCL {
    namespace CSC8503 {
        class ThirdPersonCamera {
        public:
            /**
             * Constructor
             * @param cam        Pointer to the scene camera 
             * @param controller The keyboard/mouse controller handling user input.
             */
            ThirdPersonCamera(Camera* cam, KeyboardMouseController& controller);
            ~ThirdPersonCamera();

         
            //Set the target object to follow.

            void SetFollowObject(GameObject* obj);


            //Set the offset vector from the target's position.
            void SetOffset(const Vector3& offset);
            
            void Update(float dt);
            
            //Optionally expose pitch / yaw if you want manual overrides.
            void SetPitch(float p) { pitch = p; }
            void SetYaw(float y)   { yaw   = y; }

        protected:
            Camera* camera;
            KeyboardMouseController& inputController;

            GameObject* followObject; // The object to follow.
            Vector3     offset;       // Camera offset from the target object.

            float pitch;              // Rotation around X axis.
            float yaw;                // Rotation around Y axis.
            float rotationSpeed;      // Multiplier for mouse rotation speed.
        };
    }
}
