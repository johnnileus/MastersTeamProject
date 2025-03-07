#ifdef _WIN32



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

	int id;
	Vector3 pos;
	Quaternion rot;
	bool fromServer;

	TransformPacket(int ID, const Vector3 p, const Quaternion r, bool b) {
		type = BasicNetworkMessages::Transform_Data;
		size = sizeof(Vector3) + sizeof(Quaternion) + sizeof(bool) + sizeof(int);
		id = ID;
		pos = p;
		rot = r;
		fromServer = b;
	}
};

struct ConnectPacket : public GamePacket {
	int id;
	Vector3 pos;
	Quaternion rot;

	ConnectPacket(int ID, const Vector3 p, const Quaternion r) {
		type = BasicNetworkMessages::Connected;
		size = sizeof(Vector3) + sizeof(Quaternion) + sizeof(int);
		id = ID;
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

	void ClientSendPacket(GamePacket& p);
	void ServerSendPacket(GamePacket& p, ENetPeer* peer);
	void ServerBroadcastPacket(GamePacket& p);

	int GetID();

	void OnPlayerConnected(int id);

protected:



	MainPacketReceiver networkReceiver{ "receiver" };
	bool connected = false;
	bool isServer = false;

	GameServer* server;
	GameClient* client;


	int port = NetworkBase::GetDefaultPort();

};
#endif // WIN32

