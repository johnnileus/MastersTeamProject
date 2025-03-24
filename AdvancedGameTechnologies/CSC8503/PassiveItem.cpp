//init gameobject of type item
//oncontactbegin change stats of player

#pragma once
#include "PassiveItem.h"
#include "AssetManager.h"
#include "PhysicsObject.h"

#include "json/json11.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace NCL;
using namespace CSC8503;

PassiveItem::PassiveItem(Player* player, GameWorld* world) {
	//INIT JSON FILE
	std::ifstream file("items.json");
	if (!file) {
		std::cout << "Could not load json!" << std::endl;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	std::string err;
	json11::Json json = json11::Json::parse(buffer.str(), err);
	std::cout << "JSON data: " << json.dump() << "\n";

	myWorld = world;
	myPlayer = player;
	size = 1;
	mass = 10.0f;

	name = "passive";
	tag = "Passive";
	maxSpeed = 2;//change to json value
	SetComponent(size, mass);
}

PassiveItem::~PassiveItem() {
	delete boundingVolume;
	delete physicsObject;
	delete renderObject;
	delete networkObject;
}

void PassiveItem::SetComponent(float meshSize, float inverseMass) {
	SphereVolume* volume = new SphereVolume(meshSize);
	SetBoundingVolume((CollisionVolume*)volume);

	GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize));

	SetRenderObject(new RenderObject(&GetTransform(),
		AssetManager::Instance().sphereMesh,
		AssetManager::Instance().basicTex,
		AssetManager::Instance().basicShader));

	SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));
	GetPhysicsObject()->SetInverseMass(inverseMass);
	GetPhysicsObject()->InitSphereInertia();
}

void PassiveItem::SetUid(int uid) {
	myUid = uid;
}

void PassiveItem::Init() {
	return;
}

PassiveItem* PassiveItem::Instantiate(GameWorld* world, std::vector<PassiveItem*> itemList, Player* player, const Vector3& position, int uid) {
	PassiveItem* passive = new PassiveItem(player, world);

	passive->GetTransform().SetPosition(position);
	passive->Init();
	passive->SetUid(uid);
	itemList.push_back(passive);

	world->AddGameObject(passive);

	return passive;
}

//update stats
void PassiveItem::UpdateStats(Player* player, int myUid) {
	//change to json data
	if (myUid == 2) {
		maxSpeed += myPlayer->GetSpeed();
		myPlayer->SetSpeed(maxSpeed);
	}
	else {
		return;
	}
}

void PassiveItem::OnCollisionBegin(GameObject* otherObject) {
	if (otherObject->tag == "Player") {
		otherObject->SetActive(false);
		UpdateStats(myPlayer, myUid);
	}
}