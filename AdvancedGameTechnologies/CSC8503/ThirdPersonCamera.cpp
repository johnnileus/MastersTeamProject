#include "ThirdPersonCamera.h"

#include "Debug.h"
#include "Quaternion.h"
#include "Window.h" // For mouse input
#include  "random"
using namespace NCL;
using namespace CSC8503;

ThirdPersonCamera::ThirdPersonCamera(Camera* cam, KeyboardMouseController& controller)
    : camera(cam), inputController(controller) {
    followObject   = nullptr;
    orbitRadius    = 10.0f;   // Camera distance from the target
    pitch          = -15.0f;  // Initial pitch angle
    yaw            = 0.0f;    // Initial yaw angle
    rotationSpeed  = 2.0f;    // Mouse sensitivity

    shakeDuration  = 0;
    shakeStrength  = 0;
    shakeTime      = 0;
}

ThirdPersonCamera::~ThirdPersonCamera() {
}

void ThirdPersonCamera::SetFollowObject(GameObject* obj) {
    followObject = obj;
}

void ThirdPersonCamera::Update(float dt) {
    // Get mouse input
    Vector2 mouseDelta = Window::GetMouse()->GetRelativePosition();

    // Adjust yaw and pitch based on delta time
    yaw   -= mouseDelta.x * rotationSpeed * dt * 100;
    pitch -= mouseDelta.y * rotationSpeed * dt * 100;

    // Clamp pitch to prevent flipping
    float pitchMin = -80.0f;
    float pitchMax =  80.0f;
    pitch = std::clamp(pitch, pitchMin, pitchMax);

    // Convert pitch & yaw to quaternion
    Quaternion orientation = Quaternion::EulerAnglesToQuaternion(pitch, yaw, 0.0f);

    front =Vector::Normalise(orientation*Vector3(0,0,-1));
    right = Vector::Normalise(orientation*Vector3(1,0,0));
    
    // Default offset (camera behind the target)
    Vector3 offset(1.0f, 2.0f, orbitRadius);

    // Apply rotation
    Vector3 rotatedOffset = orientation * offset;

    // Compute final camera position
    Vector3 targetPos = followObject->GetTransform().GetPosition();
    Vector3 cameraPos = targetPos + rotatedOffset;

    // Set camera position and rotation
    camera->SetPosition(cameraPos);
    camera->SetYaw(yaw);
    camera->SetPitch(pitch);

    UpdateShakeTimer(dt);
}

/// Shake camera
/// @param strength shake amplitude
/// @param duration shake duration
void ThirdPersonCamera::Shake(float strength, float duration) {
    shakeStrength = strength;
    shakeDuration  = duration;
    shakeTime      = duration;  // reset timer
}

/// Update camera shake function
/// @param dt 
void ThirdPersonCamera::UpdateShakeTimer(float dt)
{
    if (shakeTime > 0.0f) {
        
        float currentAmplitude = shakeStrength * (shakeTime / shakeDuration);

        // generate random number
        static std::default_random_engine rng{ std::random_device{}() };
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

        // get a random offset and multiply the strength
        Vector3 randomOffset(dist(rng), dist(rng), dist(rng));

        // smooth shake
        if (Vector::Length(randomOffset) > 1e-5f) {
            Vector::Normalise(randomOffset);
        }
        randomOffset *= currentAmplitude;

        //set camera position 
        Vector3 camPos= camera->GetPosition()+ randomOffset;
        camera->SetPosition(camPos);

        // update remaining time
        shakeTime -= dt;
        if (shakeTime < 0.0f) {
            shakeTime = 0.0f;
        }
    }
}


