#ifdef _WIN32



#include "GameNet.h"
#include "TutorialGame.h"

extern TutorialGame* g;




void MainPacketReceiver::ReceivePacket(int type, GamePacket* payload, int source) {
	if (type == String_Message) {
		StringPacket* realPacket = (StringPacket*)payload;
		std::string msg = realPacket->GetStringFromData();
		std::cout << "Client: String_Message received: " << msg << std::endl;
	}
	if (type == Transform_Data) {
		std::cout << "Client: Transform Received" << std::endl;
		TransformPacket* realPacket = (TransformPacket*)payload;
		g->UpdateTransformFromServer(realPacket->pos, realPacket->rot);
	}
}


void NetworkManager::StartAsServer() {
	isServer = true;
	id = 0;
	server = new GameServer(port, 1);

	server->RegisterPacketHandler(String_Message, &networkReceiver);
	server->RegisterPacketHandler(Transform_Data, &networkReceiver);


	connected = true;
}

void NetworkManager::StartAsClient() {
	client = new GameClient();
	client->RegisterPacketHandler(String_Message, &networkReceiver);
	client->RegisterPacketHandler(Transform_Data, &networkReceiver);
	bool canConnect = client->Connect(127, 0, 0, 1, port);
	if (canConnect) {
		connected = true;

	}
	else {
		std::cout << "Failed to connect client to server!" << std::endl;

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
			//std::cout << "sending msg" << std::endl;

			//client->SendPacket(clientPacket);

			client->UpdateClient();
		}
	}

}


void NetworkManager::BroadcastPacket(TransformPacket p) {
	if (IsServer()) {

		std::cout << "sending msg" << std::endl;

		server->SendGlobalPacket(p);
	}
}
#endif // WIN32