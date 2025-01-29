#include "Door.h"

#include <Color.h>

#include "AABBVolume.h"
#include "RenderObject.h"
#include "SphereVolume.h"

using namespace NCL;
using namespace CSC8503;

Door::Door()
{
    tag="Door";
    isMoving=false;
    isMovingUp=true;
    originalHeight =0;
    moveSpeed=5.0f;
    minHeight=-10.0f;
}

void Door::Init()
{
    renderObject->SetColour(Vector4(1,0.537f,0.537f,1));
    doorObject->GetRenderObject()->SetColour(Vector4(0.8,0.8,0.8,1));
}

void Door::Update(float dt) {
    if (isMoving && doorObject) {
        Vector3 currentPosition = doorObject->GetTransform().GetPosition();

        if (isMovingUp) {
            // moving up
            if (currentPosition.y < originalHeight) {
                currentPosition.y += moveSpeed * dt;
                doorObject->GetTransform().SetPosition(currentPosition);
            } else {
                isMoving = false; // 到达顶部，停止移动
            }
        } else {
            // moving down
            if (currentPosition.y > minHeight) {
                currentPosition.y -= moveSpeed * dt;
                doorObject->GetTransform().SetPosition(currentPosition);
            } else {
                isMoving = false; // 到达底部，停止移动
            }
        }
    }
}



void Door::OnCollisionBegin(GameObject* otherObject) {
    if (otherObject->tag == "Player") {
        std::cout << "!!!!!Hit Door!!!!!" << std::endl;

        if (!isMoving) {
            isMovingUp = !isMovingUp; // switch direction
            isMoving = true;         // active move
        }
    }
}



Door::~Door()
{
    
}


