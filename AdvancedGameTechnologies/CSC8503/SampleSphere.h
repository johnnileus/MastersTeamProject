#pragma once

#include "GameObject.h"

namespace NCL {
    namespace CSC8503 {

        // A simple sphere game object that configures its own collision, rendering, and physics.
        class SampleSphere : public GameObject {
        public:
            // Constructor: set up sphere properties.
            SampleSphere(float radius = 1.0f, float inverseMass = 1.0f);

            // Copy constructor used by SceneManager::Instantiate.
            SampleSphere(const SampleSphere& other);

            ~SampleSphere();
        };

    }
}
