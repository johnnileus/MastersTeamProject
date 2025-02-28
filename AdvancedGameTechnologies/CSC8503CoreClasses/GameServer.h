#pragma once
#include "NetworkBase.h"


#include "../CSC8503CoreClasses/Event.h"

namespace NCL {
	namespace CSC8503 {
		class GameWorld;
		class GameServer : public NetworkBase {
		public:
			GameServer(int onPort, int maxClients);
			~GameServer();

			bool Initialise();
			void Shutdown();

			void SetGameWorld(GameWorld &g);

			bool SendGlobalPacket(int msgID);
			bool SendGlobalPacket(GamePacket& packet);

			virtual void UpdateServer();

			Event<int> PlayerConnected;

		protected:
			int			port;
			int			clientMax;
			int			clientCount;
			GameWorld*	gameWorld;



		};



	}
}

