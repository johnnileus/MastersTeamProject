#pragma once

#include "GameObject.h"
#include "GameWorld.h"

namespace NCL {
    namespace CSC8503 {
        
        class SampleSphere : public GameObject {
        public:
            // Constructor: set up sphere properties.
            SampleSphere();

            static SampleSphere* Instantiate(GameWorld* world,
                                            const Vector3& position,
                                            const Quaternion& rotation);

            ~SampleSphere();
        };

    }
}
