#pragma once
#include "Player.h"
#include "GameWorld.h"
#include "json/json11.hpp"

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
			json11::Json jsonFile;

			bool ReadFile(const std::string& filename);
			void FindItem(Player* player, int uid);

			void SetComponent(float meshSize, float InverseMass);
			void SetUid(int uid);
			void UpdateStats(Player* player, int myUid);
			void UpdateHealth(Player* player, int healthVal);
			void UpdateDamage(Player* player, int damageVal);
			void UpdateSpeed(Player* player, int speedVal);
			float size;
			float mass;
			int myUid;

			int health;
			int damage;
			float maxSpeed;

			int healthVal;
			int damageVal;
			float speedVal;
		};
	}
}