#include "ThirdPersonCamera.h"

#include "Debug.h"
#include "Quaternion.h"
#include "Window.h" // For mouse input
#include  "random"
using namespace NCL;
using namespace CSC8503;

#ifdef USEAGC
ThirdPersonCamera::ThirdPersonCamera(Camera* cam, NCL::PS5::PS5Controller& controller)
    : camera(cam), inputController(controller) {
#else
ThirdPersonCamera::ThirdPersonCamera(Camera * cam, KeyboardMouseController& controller)
    : camera(cam), inputController(controller) {
#endif // USEAGC


    followObject   = nullptr;
    orbitRadius    = 10.0f;   // Camera distance from the target
    pitch          = -15.0f;  // Initial pitch angle
    yaw            = 0.0f;    // Initial yaw angle
#ifdef USEAGC
	rotationSpeed = 1.0f;    // Controller sensitivity
#else
    rotationSpeed = 2.0f;    // Controller sensitivity
#endif // USEAGC

    
    std::cout << "ThirdPersonCamera constructor called" << std::endl;
    //std::cout << "this pointer: " << reinterpret_cast<uintptr_t>(this) << std::endl;

    shakeDuration  = 0;
    shakeStrength  = 0;
    shakeTime      = 0;
}

ThirdPersonCamera::~ThirdPersonCamera() {
    std::cout << "ThirdPersonCamera destructor called" << std::endl;
    //std::cout << "this pointer: " << reinterpret_cast<uintptr_t>(this) << std::endl;
}

void ThirdPersonCamera::SetFollowObject(GameObject* obj) {
    //std::cout << "SetFollowObject called" << std::endl;
	std::cout << obj->GetName() << std::endl;
    //std::cout << "obj pointer: " << reinterpret_cast<uintptr_t>(obj) << std::endl;
    //std::cout << "this pointer: " << reinterpret_cast<uintptr_t>(this) << std::endl;
    if (obj == nullptr) {
        std::cout << "Error: obj is a null pointer!" << std::endl;
    }
    //std::cout << "followObject pointer before assignment: " << reinterpret_cast<uintptr_t>(followObject) << std::endl;
    followObject = obj;
    //std::cout << "followObject pointer after assignment: " << reinterpret_cast<uintptr_t>(followObject) << std::endl;
}

void ThirdPersonCamera::Update(float dt) {
#ifdef _WIN32
    // Get mouse input
    Vector2 mouseDelta = Window::GetMouse()->GetRelativePosition();
    // Adjust yaw and pitch based on delta time
    yaw   -= mouseDelta.x * rotationSpeed * dt * 100;
    pitch -= mouseDelta.y * rotationSpeed * dt * 100;
#elif (USEAGC)
    // Get controller input
    float rightStickX = inputController.GetAxis(2);
    float rightStickY = inputController.GetAxis(3);
    //Debug::Print("Right Stick: " + std::to_string(rightStickX) + ", " + std::to_string(rightStickY), Vector2(5, 70));
	//Debug::Print("rightStickX: " + std::to_string(rightStickX) + ", rightStickY: " + std::to_string(rightStickY), Vector2(5, 85));
    //std::cout << "rightStickX: " << rightStickX << ", rightStickY: " << rightStickY << std::endl;
    // Adjust yaw and pitch based on delta time
    yaw   -= rightStickX * rotationSpeed * dt * 100;
    pitch -= rightStickY * rotationSpeed * dt * 100;
#endif

    // Clamp pitch to prevent flipping
    float pitchMin = -80.0f;
    float pitchMax =  80.0f;
    pitch = std::clamp(pitch, pitchMin, pitchMax);

    // Convert pitch & yaw to quaternion
    Quaternion orientation = Quaternion::EulerAnglesToQuaternion(pitch, yaw, 0.0f);

    front = Vector::Normalise(orientation * Vector3(0, 0, -1));
    right = Vector::Normalise(orientation * Vector3(1, 0, 0));
    
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


