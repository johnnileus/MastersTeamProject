#pragma once
#include "json/json11.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

#include "PassiveItem.h"
#include "AssetManager.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

PassiveItem::PassiveItem(Player* player, GameWorld* world) {
	ReadFile("items.json");

	myWorld = world;
	myPlayer = player;
	size = 1;
	mass = 10.0f;

	name = "passive";
	tag = "Passive";

	health = 120;
	damage = 50;
	maxSpeed = 5;//change to json value

	SetComponent(size, mass);
}

PassiveItem::~PassiveItem() {
	delete boundingVolume;
	delete physicsObject;
	delete renderObject;
	delete networkObject;
}

bool PassiveItem::ReadFile(const std::string& filename) {
	//INIT JSON FILE
	std::ifstream file("items.json");
	if (!file) {
		std::cout << "Could not load json!" << std::endl;
		return false;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	std::string err;
	jsonFile = json11::Json::parse(buffer.str(), err);
	std::cout << "JSON data: " << jsonFile.dump() << endl;

	return true;
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

void PassiveItem::UpdateCall() {
	//UpdateStats(myPlayer, myUid);
	FindItem(myPlayer, myUid);
}

void PassiveItem::FindItem(Player* player, int myUid) {
	for (const auto& item : jsonFile["items"].array_items()) {
		if (item["uid"].is_number() && item["uid"].int_value() == myUid) {
			//std::cout << "match" << endl;
			if (item["health"].is_number() && item["health"].int_value() != 0) {
				healthVal = item["health"].int_value();
				UpdateHealth(player, healthVal);
			}
			if (item["damage"].is_number() && item["damage"].int_value() != 0) {
				damageVal = item["damage"].int_value();
				UpdateDamage(player, damageVal);
			}
			if (item["maxSpeed"].is_number() && item["maxSpeed"].int_value() != 0) {
				speedVal = item["maxSpeed"].number_value();
				UpdateSpeed(player, speedVal);
			}

			return;
		}
	}
}

PassiveItem* PassiveItem::Instantiate(GameWorld* world, std::vector<PassiveItem*> itemList, Player* player, const Vector3& position, int uid) {
	PassiveItem* passive = new PassiveItem(player, world);

	passive->GetTransform().SetPosition(position);
	passive->SetUid(uid);
	itemList.push_back(passive);

	world->AddGameObject(passive);

	return passive;
}

//update stats deprecated
/*
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
*/

void PassiveItem::UpdateHealth(Player* player, int healthVal) {
	health = healthVal + myPlayer->GetHealth();
	myPlayer->SetHealth(health);
}

void PassiveItem::UpdateDamage(Player* player, int damageVal) {
	damage = damageVal + myPlayer->GetDamage();
	myPlayer->SetDamage(damage);
}

void PassiveItem::UpdateSpeed(Player* player, int speedVal) {
	maxSpeed = speedVal + myPlayer->GetSpeed();
	myPlayer->SetSpeed(maxSpeed);
}