#pragma once

#include <iostream>

#include "GameServer.h"
#include "GameClient.h"
#include "NetworkBase.h"
#include <string>


#include "Quaternion.h"
#include "Vector.h"

using namespace NCL;
using namespace CSC8503;
using namespace NCL::Maths;

struct TransformPacket : public GamePacket {
	Vector3 pos;
	Quaternion rot;

	TransformPacket(const Vector3 p, const Quaternion r) {
		type = BasicNetworkMessages::Transform_Data;
		size = sizeof(Vector3) + sizeof(Quaternion);
		pos = p;
		rot = r;
	}


};


class MainPacketReceiver : public PacketReceiver {
public:
	MainPacketReceiver(std::string name) {
		this->name = name;
	}

	void ReceivePacket(int type, GamePacket* payload, int source);
	int score;

protected:
	std::string name;
};

class NetworkManager {
public:

	NetworkManager() {
		NetworkBase::Initialise();
	}

	void StartAsServer();

	void StartAsClient();

	bool IsServer();
	bool IsClient();
	bool IsConnected();

	void Update();

	void BroadcastPacket(TransformPacket p);

protected:

	MainPacketReceiver networkReceiver{ "receiver" };
	bool connected = false;
	bool isServer = false;

	GameServer* server;
	GameClient* client;

	int id;

	int port = NetworkBase::GetDefaultPort();

};


