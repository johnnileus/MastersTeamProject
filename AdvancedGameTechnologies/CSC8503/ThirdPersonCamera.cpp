#include "ThirdPersonCamera.h"
#include "Quaternion.h"
#include "Maths.h"
#include "Window.h" // 用于获取鼠标输入

using namespace NCL;
using namespace CSC8503;

ThirdPersonCamera::ThirdPersonCamera(Camera* cam, KeyboardMouseController& controller)
    : camera(cam), inputController(controller) {
    followObject   = nullptr;
    offset         = Vector3(0, 10, -20); // 典型的第三人称偏移量
    pitch          = -15.0f;
    yaw            = 0.0f;
    rotationSpeed  = 10.0f;
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
    if (!camera || !followObject) {
        return;
    }
    
    // 读取鼠标输入（假设 GetRelativePosition() 返回的是上一帧的位移）
    float yawInput   = Window::GetMouse()->GetRelativePosition().x;
    float pitchInput = Window::GetMouse()->GetRelativePosition().y;
    
    // 更新 yaw 与 pitch，乘以 dt 以保证旋转速度与帧率无关
    yaw   += yawInput   * rotationSpeed * dt;
    pitch += pitchInput * rotationSpeed * dt;
    
    // 限制 pitch 范围，防止过大导致翻转
    if (pitch < -80.0f) { 
        pitch = -80.0f;
    }
    if (pitch >  80.0f) { 
        pitch = 80.0f;
    }
    
    // 将角度转换为弧度
    float yawRad   = Maths::DegreesToRadians(yaw);
    float pitchRad = Maths::DegreesToRadians(pitch);
    
    // 固定旋转半径
    float r = 20;  // 如果没有 orbitRadius 成员变量，可直接写例如： float r = 20.0f;
    
    // 根据球坐标公式计算相机相对于玩家的偏移量  
    // 注意：我们希望当 yaw=0 且 pitch=0 时，相机位于玩家后方，即 (0,0,-r)
    float cosPitch = cos(pitchRad);
    float sinPitch = sin(pitchRad);
    float cosYaw   = cos(yawRad);
    float sinYaw   = sin(yawRad);
    
    // 计算偏移向量：  
    // x = r * cos(pitch) * sin(yaw)  
    // y = r * sin(pitch)  
    // z = -r * cos(pitch) * cos(yaw)
    Vector3 offset( r * cosPitch * sinYaw,  r * sinPitch,  -r * cosPitch * cosYaw );
    
    // 获取玩家（跟随对象）的位置
    Vector3 playerPos = followObject->GetTransform().GetPosition();
    
    // 计算最终相机位置：以玩家位置为中心，加上计算得到的偏移量
    Vector3 cameraPos = playerPos + offset;
    
    // 更新相机位置
    camera->SetPosition(cameraPos);
    
    // 使相机始终朝向玩家
    camera->LookAt(playerPos);
    
}


