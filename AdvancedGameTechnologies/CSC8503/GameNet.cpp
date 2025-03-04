#include <iostream>

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
		if (!realPacket->fromServer) {
			g->UpdateConnectedPlayer(source, realPacket->pos, realPacket->rot);
		}
		else { // packet came from server
		}
	}
	if (type == Connected) {
		ConnectPacket* realPacket = (ConnectPacket*)payload;
		std::cout << "New Player" << realPacket->id << std::endl;

		g->InitialiseConnectedPlayerObject(realPacket->id);
	}
}

//only called by server
void NetworkManager::OnPlayerConnected(int id) {
	ENetPeer* connectedClients = server->GetConnectedPeers();
	
	ENetPeer* currentPeer;
	_ENetHost* host = server->getNetHandle();
	for (currentPeer = host->peers; currentPeer < &host->peers[host->peerCount]; ++currentPeer)
	{
		if (currentPeer->state == ENET_PEER_STATE_CONNECTED) {

			ConnectPacket transform(id, Vector3(), Quaternion());

			SendPacket(transform);
		}
	}



	g->InitialiseConnectedPlayerObject(id);
}

void NetworkManager::StartAsServer() {
	if (!IsConnected()) {
		isServer = true;
		server = new GameServer(port, 8);
		server->PlayerConnected.AddListener(std::bind(&NetworkManager::OnPlayerConnected, this, std::placeholders::_1));

		server->RegisterPacketHandler(String_Message, &networkReceiver);
		server->RegisterPacketHandler(Transform_Data, &networkReceiver);
		server->RegisterPacketHandler(ID, &networkReceiver);
		server->RegisterPacketHandler(Connected, &networkReceiver);
	}


	connected = true;
}

void NetworkManager::StartAsClient() {
	if (!IsConnected()) {

		client = new GameClient();
		client->RegisterPacketHandler(String_Message, &networkReceiver);
		client->RegisterPacketHandler(Transform_Data, &networkReceiver);
		client->RegisterPacketHandler(ID, &networkReceiver);
		client->RegisterPacketHandler(Connected, &networkReceiver);
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
int NetworkManager::GetID() {
	return client->GetID();
}
void NetworkManager::Update() {
	if (connected) {

		if (isServer) {


			//g->BroadcastPosition();

			server->UpdateServer();
		}
		else {

			//StringPacket clientPacket("Client says hello!");

			//client->SendPacket(clientPacket);


			g->SendTransform();

			//client->GetID();
			client->UpdateClient();
		}
	}

}



void NetworkManager::SendPacket(GamePacket& p) {
	if (IsServer()) {
		server->SendGlobalPacket(p);
	}
	else if (IsClient()) {
		client->SendPacket(p);
	}
}


// -- TUTORIAL GAME --

GameObject* TutorialGame::InitialiseConnectedPlayerObject(int id) {

	float meshSize = 1.0f;
	float inverseMass = 10.0f;

	GameObject* newPlayer = new GameObject();
	SphereVolume* volume = new SphereVolume(1.0f);

	newPlayer->SetBoundingVolume((CollisionVolume*)volume);

	newPlayer->SetBoundingVolume((CollisionVolume*)volume);

	newPlayer->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(Vector3(20, 0, 30));

	newPlayer->SetRenderObject(new RenderObject(&newPlayer->GetTransform(), AssetManager::Instance().sphereMesh, AssetManager::Instance().metalTex, AssetManager::Instance().basicShader));
	newPlayer->SetPhysicsObject(new PhysicsObject(&newPlayer->GetTransform(), newPlayer->GetBoundingVolume()));

	newPlayer->GetPhysicsObject()->SetInverseMass(inverseMass);
	newPlayer->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(newPlayer);
	connectedPlayers[id] = newPlayer;

	return newPlayer;

}

void TutorialGame::SendTransform() {
	if (networkManager->IsClient()) {
		Vector3 pos = player->GetTransform().GetPosition();
		Quaternion rot = player->GetTransform().GetOrientation();

		int id = networkManager->GetID();

		TransformPacket transform(id, pos, rot, false);

		networkManager->SendPacket(transform);
	}

}

void TutorialGame::BroadcastPosition() {

	Vector3 pos = player->GetTransform().GetPosition();
	Quaternion rot = player->GetTransform().GetOrientation();


	TransformPacket transform(-1, pos, rot, true);

	networkManager->SendPacket(transform);
}

void TutorialGame::UpdateTransformFromServer(Vector3 pos, Quaternion rot) {
	player->GetTransform().SetOrientation(rot);
	player->GetTransform().SetPosition(pos);
}

void TutorialGame::UpdateConnectedPlayer(int id, Vector3 pos, Quaternion rot) {
	GameObject* plr = connectedPlayers[id];
	plr->GetTransform().SetOrientation(rot);
	plr->GetTransform().SetPosition(pos);
}