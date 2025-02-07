#include "ThirdPersonCamera.h"

#include "Window.h"

using namespace NCL;
using namespace CSC8503;

ThirdPersonCamera::ThirdPersonCamera(Camera* cam, KeyboardMouseController& controller)
    : camera(cam), inputController(controller) {
    followObject   = nullptr;
    offset         = Vector3(0, 10, 25); // Default offset
    pitch          = -15.0f;            // Default pitch
    yaw            = 315.0f;            // Default yaw
    rotationSpeed  = 5.0f;              // Rotation speed factor
}

ThirdPersonCamera::~ThirdPersonCamera() {
}

void ThirdPersonCamera::SetFollowObject(GameObject* obj) {
    followObject = obj;
}

void ThirdPersonCamera::SetOffset(const Vector3& o) {
    offset = o;
}

void ThirdPersonCamera::Update(float dt) {
    if (!camera) {
        return; 
    }
    
    //Read mouse input from the mapped axes
    //float yawInput   = inputController.GetAxis("XLook");
    //float pitchInput = inputController.GetAxis("YLook");
    float yawInput =Window::GetMouse()->GetRelativePosition().x;
    float pitchInput = Window::GetMouse()->GetRelativePosition().y;


    // invert pitch input because moving the mouse up means pitch should decrease.
    yaw   += yawInput   * rotationSpeed;
    pitch -= pitchInput * rotationSpeed;

    // Clamp pitch to avoid extreme angles
    if (pitch < -80.0f) {
        pitch = -80.0f;
    }
    if (pitch > 80.0f) {
        pitch = 80.0f;
    }

    // If it has a follow object, perform the third-person camera logic
    if (followObject) {
        // Get the object position
        Vector3 objPos = followObject->GetTransform().GetPosition();

        // Convert degrees to radians (if your pitch and yaw are stored in degrees)
        float yawRad   = Maths::DegreesToRadians(yaw);
        float pitchRad = Maths::DegreesToRadians(pitch);

        // Yaw rotation (around Y axis)
        // offsetYRotated = Ry(yawRad) * offset
        Vector3 offsetYRotated;
        offsetYRotated.x = offset.x * std::cos(yawRad) + offset.z * std::sin(yawRad);
        offsetYRotated.y = offset.y;
        offsetYRotated.z = offset.z * std::cos(yawRad) - offset.x * std::sin(yawRad);

        // pitch rotation (around X axis)
        // finalOffset = Rx(pitchRad) * offsetYRotated
        Vector3 finalOffset;
        finalOffset.x = offsetYRotated.x;
        finalOffset.y = offsetYRotated.y * std::cos(pitchRad) - offsetYRotated.z * std::sin(pitchRad);
        finalOffset.z = offsetYRotated.z * std::cos(pitchRad) + offsetYRotated.y * std::sin(pitchRad);

        //Compute camera position
        Vector3 camPos = objPos + finalOffset;

        //update camera with final values
        camera->SetPosition(camPos);
        camera->SetPitch(pitch);
        camera->SetYaw(yaw);
    }
    else {
        
    }
}
