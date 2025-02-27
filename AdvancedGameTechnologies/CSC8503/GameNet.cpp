#ifdef _WIN32



#include "GameNet.h"
#include "TutorialGame.h"

extern TutorialGame* g;




void MainPacketReceiver::ReceivePacket(int type, GamePacket* payload, int source) {
	if (type == String_Message) {
		StringPacket* realPacket = (StringPacket*)payload;
		std::string msg = realPacket->GetStringFromData();
	}
	if (type == Transform_Data) {
		TransformPacket* realPacket = (TransformPacket*)payload;
		g->UpdateTransformFromServer(realPacket->pos, realPacket->rot);
	}
}

void printstuff(int i) {
	std::cout << "Hello" << std::endl;
}

void NetworkManager::StartAsServer() {
	if (!IsConnected()) {
		isServer = true;
		server = new GameServer(port, 8);
		server->Player_Connected.AddListener(std::bind(&TutorialGame::InitialiseConnectedPlayer, g, std::placeholders::_1));

		server->RegisterPacketHandler(String_Message, &networkReceiver);
		server->RegisterPacketHandler(Transform_Data, &networkReceiver);
		server->RegisterPacketHandler(ID, &networkReceiver);
	}


	connected = true;
}

void NetworkManager::StartAsClient() {
	if (!IsConnected()) {

		client = new GameClient();
		client->RegisterPacketHandler(String_Message, &networkReceiver);
		client->RegisterPacketHandler(Transform_Data, &networkReceiver);
		client->RegisterPacketHandler(ID, &networkReceiver);
		bool canConnect = client->Connect(127, 0, 0, 1, port);
		if (canConnect) {
			connected = true;

		}
		else {
			std::cout << "Failed to connect client to server!" << std::endl;
		}
	}
}

bool NetworkManager::IsServer() {
	if (connected && isServer) { return true; }
	return false;
}
bool NetworkManager::IsClient() {
	if (connected && !isServer) { return true; }
	return false;
}
bool NetworkManager::IsConnected() {
	return connected;
}

void NetworkManager::Update() {
	if (connected) {

		if (isServer) {


			g->BroadcastPosition();

			server->UpdateServer();
		}
		else {

			//StringPacket clientPacket("Client says hello!");

			//client->SendPacket(clientPacket);

			client->GetID();
			client->UpdateClient();
		}
	}

}


void NetworkManager::BroadcastPacket(TransformPacket p) {
	if (IsServer()) {


		server->SendGlobalPacket(p);
	}
}
#endif // WIN32