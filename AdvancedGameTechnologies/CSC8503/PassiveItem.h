#pragma once
#include "Player.h"
#include "GameWorld.h"

//Getters and Setters for Player stats needed

namespace NCL {
	namespace CSC8503 {
		class PassiveItem : public GameObject {
		public:
			PassiveItem(Player* player, GameWorld* world);
			~PassiveItem();

			void UpdateCall();

			static PassiveItem* Instantiate(GameWorld* world, std::vector<PassiveItem*> itemList, Player* player, const Vector3& position, int uid);

			GameWorld* myWorld;

			Player* myPlayer;
		protected:
			void SetComponent(float meshSize, float InverseMass);
			void SetUid(int uid);
			void UpdateStats(Player* player, int myUid);
			float size;
			float mass;
			int myUid;

			float maxSpeed;
			int health;
		};
	}
}