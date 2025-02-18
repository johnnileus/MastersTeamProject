#include "CatCoin.h"
using namespace NCL;
using namespace CSC8503;

CatCoin::CatCoin()
{
    tag = "Coin";
    name = "Coin";
    
    float meshSize		= 2.0f;
    float inverseMass	= 25.0f;

    // Transform
    GetTransform().SetScale(Vector3(meshSize,meshSize,meshSize));
    
    //Collider
    SphereVolume* volume = new SphereVolume(meshSize-1);
    SetBoundingVolume((CollisionVolume*)volume);
    SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));
    GetPhysicsObject()->SetInverseMass(inverseMass);
    GetPhysicsObject()->InitSphereInertia();
    
    
    //Render
    SetRenderObject(new RenderObject(&GetTransform(),
        AssetManager::Instance().bonusMesh, 
        nullptr,   
        AssetManager::Instance().basicShader
    ));
    
}

void CatCoin::Init()
{
    renderObject->SetColour(Vector4(1,0.988,0.396,1) );
}

CatCoin* CatCoin::Instantiate(GameWorld* world, const Vector3& position) {
    
    CatCoin* coin = new CatCoin();

    // Set position and Rotation
    coin->GetTransform().SetPosition(position);
    coin->GetTransform().SetOrientation(Quaternion(Vector3(0,0,0),0));
    
    // Add to GameWorld
    if (world) {
        world->AddGameObject(coin);
    }
    coin->Init();
    return coin;
}

CatCoin::~CatCoin()
{
    
}


