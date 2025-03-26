#include "Door.h"

#include <Color.h>

#include "AABBVolume.h"
#include "AssetManager.h"
#include "RenderObject.h"
#include "SphereVolume.h"
#include "PhysicsObject.h"
#include "AudioManager.h"

using namespace NCL;
using namespace CSC8503;

//A `Door` instance contains a `Trigger` and a `Door panel`.
Door::Door()
{
    tag="Door";
    isMoving=false;
    isMovingUp=true;
    originalHeight =0;
    moveSpeed=5.0f;
    minHeight=-10.0f;
    hasPlayedSound = false;

    //Collider
    AABBVolume* volume = new AABBVolume(Vector3(1,1,1));
    SetBoundingVolume((CollisionVolume*)volume);

    //Transform
    GetTransform().SetScale(Vector3(3, 6, 1));

    //Render
    SetRenderObject(new RenderObject(&GetTransform(), AssetManager::Instance().cubeMesh, nullptr, AssetManager::Instance().basicShader));

    //Physics
    SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));
    GetPhysicsObject()->SetInverseMass(0);
    GetPhysicsObject()->InitSphereInertia();
}

void Door::Init(GameObject* doorPanel)
{
    doorObject=doorPanel;
    renderObject->SetColour(Vector4(1,0.537f,0.537f,1));
    doorObject->GetRenderObject()->SetColour(Vector4(0.8,0.8,0.8,1));
    std::cout<<">>>>>>>>>>Init Door Success<<<<<<<<<<<"<<std::endl;
}

void Door::Update(float dt) {
    UpdateDoorMovement(dt);
}

void Door::UpdateDoorMovement(float dt)
{
    if (isMoving && doorObject) {
        Vector3 currentPosition = doorObject->GetTransform().GetPosition();

        if (isMovingUp) {
            // moving up
            if (currentPosition.y < originalHeight) {
                currentPosition.y += moveSpeed * dt;
                isMoving = true;
                doorObject->GetTransform().SetPosition(currentPosition);
            } else {
                isMoving = false; // The door reaches the top and stops moving
                
            }
        } else {
            // moving down
            if (currentPosition.y > minHeight) {
                currentPosition.y -= moveSpeed * dt;
                isMoving = true;
                doorObject->GetTransform().SetPosition(currentPosition);
            } else {
                isMoving = false; // The door reaches the bottom and stops moving
  
            }
        }
    }

    if (isMoving) {
        if (!hasPlayedSound) {
            
            AudioManager::GetInstance().PlayEvent("event:/DoorDown");
            hasPlayedSound = true;  
        }
    }
    else {
        
        if (hasPlayedSound) {
            hasPlayedSound = false;  
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

/// Creat a door panel 
/// @param world current world
/// @param position door panel's position
/// @param size door panel's scale
/// @return 
GameObject* Door::CreateDoorPanelObject(GameWorld* world, const Vector3& position, const Vector3& size, const Quaternion& rotation)
{
    GameObject* doorObject = new GameObject();
    doorObject->tag = "Ground";
    doorObject->SetName("DoorPanel");
    
    Vector3 floorSize = size;
    AABBVolume* volume = new AABBVolume(floorSize);
    doorObject->SetBoundingVolume((CollisionVolume*)volume);
    doorObject->GetTransform()
        .SetScale(floorSize * 2.0f)
        .SetPosition(position)
        .SetOrientation(rotation);

    doorObject->SetRenderObject(new RenderObject(&doorObject->GetTransform(),AssetManager::Instance().cubeMesh, AssetManager::Instance().floorTex, AssetManager::Instance().basicShader));
    doorObject->GetRenderObject()->SetColour(Vector4(1,1,1,1));
    doorObject->SetPhysicsObject(new PhysicsObject(&doorObject->GetTransform(), doorObject->GetBoundingVolume()));

    doorObject->GetPhysicsObject()->SetInverseMass(0);
    doorObject->GetPhysicsObject()->InitCubeInertia();
    world->AddGameObject(doorObject);
	
    return doorObject;
}

/// Create a door instance must contains a `Trigger` and a `Door panel`
/// @param world current world
/// @param trigger_pos The position of the door trigger.
/// @param panel_pos The position of the door panel.
/// @param trigger_quaternion The quaternion of the door trigger.
/// @param panel_quaternion The quaternion of the door panel.
/// @return 
Door* Door::Instantiate(GameWorld* world,
                        const Vector3& trigger_pos,
                        const Vector3& panel_pos,
                        const Quaternion& trigger_quaternion,
                        const Quaternion& panel_quaternion)
{
    Door* door = new Door();

    // Set it's location and rotation
    door->GetTransform().SetPosition(trigger_pos);
    door->GetTransform().SetOrientation(trigger_quaternion);

    GameObject* doorPanel = door->CreateDoorPanelObject(world,panel_pos, Vector3(6,9,1),panel_quaternion);

    // Add to the GameWorld
    if (world) {
        world->AddGameObject(door);
        world->AddGameObject(doorPanel);
    }
    door->Init(doorPanel);

    return door;
}


Door::~Door()
{
    
}


