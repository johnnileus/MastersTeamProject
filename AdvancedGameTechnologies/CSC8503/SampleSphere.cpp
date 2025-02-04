// SampleSphere.cpp

#include "SampleSphere.h"
#include "SphereVolume.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include "AssetManager.h"

using namespace NCL;
using namespace CSC8503;

// Constructor: Automatically adds collision, rendering, and physics
SampleSphere::SampleSphere() {

    float radius = 1.0f;
    float inverseMass = 10.0f;
    
    // Collider
    SphereVolume* volume = new SphereVolume(radius);
    SetBoundingVolume((CollisionVolume*)volume);

    // Transform
    GetTransform().SetScale(Vector3(radius,radius,radius));

    // Render
    SetRenderObject(new RenderObject(
        &GetTransform(),
        AssetManager::Instance().sphereMesh,  
        AssetManager::Instance().basicTex,    
        AssetManager::Instance().basicShader 
    ));

    // Physics
    SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));
    GetPhysicsObject()->SetInverseMass(inverseMass);
    GetPhysicsObject()->InitSphereInertia();
}

SampleSphere::~SampleSphere() {
    
}


SampleSphere* SampleSphere::Instantiate(GameWorld* world,
                                        const Vector3& position,
                                        const Quaternion& rotation) {
    
    SampleSphere* sphere = new SampleSphere();

    // Set it's location and rotation
    sphere->GetTransform().SetPosition(position);
    sphere->GetTransform().SetOrientation(rotation);

    // Add to the GameWorld
    if (world) {
        world->AddGameObject(sphere);
    }

    return sphere;
}
