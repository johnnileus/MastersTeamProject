#pragma once
#include "json11.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

#include "PassiveItem.h"
#include "AssetManager.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

std::vector<PassiveItem*>PassiveItem::itemList = {};

PassiveItem::PassiveItem(Player* player, GameWorld* world) {
	ReadFile("items.json");

	myWorld = world;
	myPlayer = player;

	myPistol = player->pistol;
	myRifle = player->rifle;
	myShotgun = player->shotGun;

	size = 1;
	mass = 10.0f;

	name = "passive";
	tag = "Passive";

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
	//std::cout << "JSON data: " << jsonFile.dump() << endl;

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

	GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));

	SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));
	GetPhysicsObject()->SetInverseMass(inverseMass);
	GetPhysicsObject()->InitSphereInertia();
}

void PassiveItem::SetUid(int uid) {
	myUid = uid;
}

void PassiveItem::UpdateCall() {
	//FindItem(myPlayer, myUid);
}

void PassiveItem::FindItem(Player* player, int myUid) {
	for (const auto& item : jsonFile["items"].array_items()) {
		if (item["uid"].is_number() && item["uid"].int_value() == myUid) {
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
			if (item["jumpForce"].is_number() && item["jumpForce"].int_value() != 0) {
				jumpVal = item["jumpForce"].number_value();
				UpdateJump(player, jumpVal);
			}

			return;
		}
	}
}

std::string PassiveItem::ShowItem() {
	FindItem(myPlayer, myUid);
	for (const auto& item : jsonFile["items"].array_items()) {
		if (item["uid"].is_number() && item["uid"].int_value() == myUid) {
			if (item["name"].is_string()) {
				itemName = item["name"].string_value();
			}
		}
	}
	return itemName;
}

PassiveItem* PassiveItem::Instantiate(GameWorld* world, Player* player, const Vector3& position, int uid) {
	PassiveItem* passive = new PassiveItem(player, world);

	passive->GetTransform().SetPosition(position);
	passive->SetUid(uid);
	itemList.push_back(passive);

	world->AddGameObject(passive);

	return passive;
}

void PassiveItem::OnCollisionBegin(GameObject* otherObject) {
	if (otherObject->tag == "Cube") {
		Vector3 currentPos = this->GetTransform().GetPosition();
		this->GetTransform().SetPosition(Vector3((std::rand() % 221) - 110, currentPos.y, (std::rand() % 221) - 110));
		//this->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
	}
}

void PassiveItem::UpdateHealth(Player* player, int healthVal) {
	health = healthVal + myPlayer->GetHealth();
	myPlayer->SetHealth(health);
}

void PassiveItem::UpdateDamage(Player* player, int damageVal) {
	playerDamage = damageVal + myPlayer->GetDamage();
	myPlayer->SetDamage(playerDamage);

	pistolDamage = damageVal + myPistol->getDamage();
	myPistol->setDamage(pistolDamage);

	rifleDamage = (damageVal * 0.2) + myRifle->getDamage();
	myRifle->setDamage(rifleDamage);

	shotgunDamage = (damageVal * 0.5) + myShotgun->getDamage();
	myShotgun->setDamage(shotgunDamage);

}

void PassiveItem::UpdateSpeed(Player* player, int speedVal) {
	maxSpeed = speedVal + myPlayer->GetSpeed();
	myPlayer->SetSpeed(maxSpeed);
}

void PassiveItem::UpdateJump(Player* player, int jumpVal) {
	jumpForce = jumpVal + myPlayer->GetJump();
	myPlayer->SetJump(jumpForce);
}