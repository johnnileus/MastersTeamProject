#pragma once
#include "Player.h"
#include "GameWorld.h"
#include "json11.hpp"

#include "Pistol.h"
#include "Rifle.h"
#include "Shotgun.h"

namespace NCL {
	namespace CSC8503 {
		class PassiveItem : public GameObject {
		public:
			PassiveItem(Player* player, GameWorld* world);
			~PassiveItem();

			void UpdateCall();

			static PassiveItem* Instantiate(GameWorld* world, Player* player, const Vector3& position, int uid);

			std::string ShowItem();
			std::string itemName;

			void OnCollisionBegin(GameObject* otherObject) override;

			static std::vector<PassiveItem*> itemList;

			GameWorld* myWorld;

			Player* myPlayer;

			Pistol* myPistol;
			Rifle* myRifle;
			Shotgun* myShotgun;
		protected:
			json11::Json jsonFile;

			bool ReadFile(const std::string& filename);
			void FindItem(Player* player, int uid);

			void SetComponent(float meshSize, float InverseMass);
			void SetUid(int uid);
			void UpdateHealth(Player* player, int healthVal);
			void UpdateDamage(Player* player, int damageVal);
			void UpdateSpeed(Player* player, int speedVal);
			void UpdateJump(Player* player, int jumpVal);
			float size;
			float mass;
			int myUid;

			int health;
			int playerDamage;//for player (player.cpp)
			float maxSpeed;
			float jumpForce;

			int pistolDamage;
			int rifleDamage;
			int shotgunDamage;

			int healthVal;
			int damageVal;
			float speedVal;
			float jumpVal;
		};
	}
}