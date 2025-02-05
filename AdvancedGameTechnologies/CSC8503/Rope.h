#pragma once
#include "SampleSphere.h"
#include "SphereVolume.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include "AssetManager.h"

namespace NCL
{
    namespace CSC8503
    {
        class Rope
        {
        public:
            Rope();
            static GameObject* AddRopeToWorld(GameWorld* world, const Vector3& startPos, const Vector3& endPos, float interval);
        };
    }
}
