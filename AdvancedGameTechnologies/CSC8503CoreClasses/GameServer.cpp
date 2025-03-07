#pragma once

#include "GameServer.h"
#include "GameWorld.h"

using namespace NCL;
using namespace CSC8503;


GameServer::GameServer(int onPort, int maxClients)	{
	port		= onPort;
	clientMax	= maxClients;
	clientCount = 0;
	netHandle	= nullptr;
	Initialise();
}

GameServer::~GameServer()	{
	Shutdown();
}

void GameServer::Shutdown() {
	SendGlobalPacket(BasicNetworkMessages::Shutdown);
	enet_host_destroy(netHandle);
	netHandle = nullptr;
}

bool GameServer::Initialise() {
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;

	netHandle = enet_host_create(&address, clientMax, 1, 0, 0);

	if (!netHandle) {
		std::cout << __FUNCTION__ << " failed to create network handle!" << std::endl;
		return false;
	}

	return true;
}


bool GameServer::SendGlobalPacket(int msgID) {
	GamePacket packet;
	packet.type = msgID;
	return SendGlobalPacket(packet);
}

bool GameServer::SendGlobalPacket(GamePacket& packet) {
	ENetPacket* dataPacket = enet_packet_create(&packet, packet.GetTotalSize(), 0);
	enet_host_broadcast(netHandle, 0, dataPacket);
	return true;
}

bool GameServer::SendPacket(GamePacket& packet, ENetPeer* peer) {
	ENetPacket* dataPacket = enet_packet_create(&packet, packet.GetTotalSize(), 0);
	enet_peer_send(peer, 0, dataPacket);

	return true;
}

void GameServer::UpdateServer() {
	if (!netHandle) { return; }
	ENetEvent event;

	while (enet_host_service(netHandle, &event, 0) > 0) {

		int type = event.type;

		ENetPeer* p = event.peer;
		int peerID = p->incomingPeerID;

		if (type == ENET_EVENT_TYPE_CONNECT) {
			std::cout << "Server: New client connected" << std::endl;


			std::cout << peerID << "<- id" << std::endl;
			PlayerConnected.Invoke(p);

				



		}
		else if (type == ENET_EVENT_TYPE_DISCONNECT) {
			std::cout << "Server: A client has disconnected" << std::endl;
		}
		else if (type == ENET_EVENT_TYPE_RECEIVE) {
			GamePacket* packet = (GamePacket*)event.packet->data;

			ProcessPacket(packet, peerID);
		}
		enet_packet_destroy(event.packet);
	}
}


void GameServer::SetGameWorld(GameWorld &g) {
	gameWorld = &g;
}

ENetPeer* GameServer::GetConnectedPeers() {
	return netHandle->peers;
}
int GameServer::GetClientCount() {
	return netHandle->peerCount;
}