#include "SampleSphere.h"
#include "SphereVolume.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include "AssetManager.h"
#include "Quaternion.h"

namespace NCL {
    namespace CSC8503 {

        SampleSphere::SampleSphere(float radius, float inverseMass) {
            // Set collision volume
            SphereVolume* volume = new SphereVolume(radius);
            SetBoundingVolume((CollisionVolume*)volume);

            // Set uniform scale based on radius.
            GetTransform().SetScale(Vector3(radius, radius, radius));

            // Set render object using asset manager resources.
            SetRenderObject(new RenderObject(
                &GetTransform(),
                AssetManager::Instance().sphereMesh,
                AssetManager::Instance().basicTex,
                AssetManager::Instance().basicShader
            ));

            // Set physics object and initialize inertia.
            SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));
            PhysicsObject* phys = GetPhysicsObject();
            phys->SetInverseMass(inverseMass);
            phys->InitSphereInertia();
        }

        SampleSphere::SampleSphere(const SampleSphere& other)
            : GameObject(other)  // Copy base class members.
        {
            // Assume uniform scale: use x component as radius.
            float radius = GetTransform().GetScale().x;

            // Recreate collision volume.
            SphereVolume* volume = new SphereVolume(radius);
            SetBoundingVolume((CollisionVolume*)volume);

            // Recreate render object.
            SetRenderObject(new RenderObject(
                &GetTransform(),
                AssetManager::Instance().sphereMesh,
                AssetManager::Instance().basicTex,
                AssetManager::Instance().basicShader
            ));

            // Recreate physics object.
            SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));
            PhysicsObject* phys = GetPhysicsObject();
            phys->SetInverseMass(other.GetPhysicsObject()->GetInverseMass());
            phys->InitSphereInertia();
        }

        SampleSphere::~SampleSphere() {
            
            
        }

    }
}
