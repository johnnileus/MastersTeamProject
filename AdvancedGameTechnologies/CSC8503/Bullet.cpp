﻿#include "Bullet.h"

#include "SceneManager.h"
#include "DemoShootableEnemy.h"

using namespace NCL;
using namespace CSC8503;

Bullet::Bullet(float damage, float speed, float distance)
{
    this->damage = damage;
    this->speed = speed;
    this->distance = distance;
    this->isDead = false;

    SetComponent(0.1,1);
}

Bullet::~Bullet()
{
    
}

/// generate a bullet
/// @param world target world
/// @param position start position
/// @param direction travel direction
/// @param weapon from witch weapon
/// @return 
Bullet* Bullet::Instantiate(GameWorld* world, const Vector3& position, const Vector3& direction, Weapon* weapon,float distance,Player* player)
{

    Bullet* bullet = new Bullet(weapon->getDamage(),200,distance);
    bullet->tag = "Bullet";
    bullet->direction = Vector::Normalise(direction);
    bullet->GetTransform().SetPosition(position); // original position
    bullet->myWorld = world;
    bullet->damage = weapon->getDamage();
    world->AddGameObject(bullet);//add to physic world
    SceneManager::Instance().AddBullet(bullet);//must add to the manager for updating
    if (player->debugMode)
    {
        Debug::DrawLine(position, position+direction*distance,Debug::CYAN,0.5);
    }
    return bullet;
}

void Bullet::Update(float dt) {
    
    float moveDist = speed * dt;  

    // update transform, follow the direction
    Vector3 currentPos = GetTransform().GetPosition();
    currentPos += direction * moveDist;
    GetTransform().SetPosition(currentPos);

    // track travelled distance
    travelled += moveDist;

    
    if (travelled >= distance) {
        isDead = true;
    }
}


void Bullet::SetComponent(float meshSize,float inverseMass)
{
    //Collider
    SphereVolume* volume  = new SphereVolume(meshSize);
    SetBoundingVolume((CollisionVolume*)volume);

    //Transform
    GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize));

    //Render
    SetRenderObject(new RenderObject(
        &GetTransform(),
        AssetManager::Instance().sphereMesh,
        AssetManager::Instance().woodTex,
        AssetManager::Instance().basicShader));

    //Physics
    SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));
    GetPhysicsObject()->SetInverseMass(inverseMass);
    GetPhysicsObject()->InitSphereInertia();

    renderObject->SetColour(Vector4(1,1,1,5));
}

void Bullet::OnCollisionBegin(GameObject* otherObject)
{
    if (otherObject->tag=="Enemy")
    {
        DemoShootableEnemy* e = dynamic_cast<DemoShootableEnemy*>(otherObject);
        e->RegisterHit(this->damage);
        myWorld->RemoveGameObject(this);
    }
}


