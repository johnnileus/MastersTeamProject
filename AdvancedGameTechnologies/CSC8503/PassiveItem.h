#pragma once
#include "Player.h"
#include "GameWorld.h"
#include "json11.hpp"

namespace NCL {
	namespace CSC8503 {
		class PassiveItem : public GameObject {
		public:
			PassiveItem(Player* player, GameWorld* world);
			~PassiveItem();

			void UpdateCall();

			static PassiveItem* Instantiate(GameWorld* world, Player* player, const Vector3& position, int uid);

			void OnCollisionBegin(GameObject* otherObject) override;

			static std::vector<PassiveItem*> itemList;

			GameWorld* myWorld;

			Player* myPlayer;
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
			int damage;
			float maxSpeed;
			float jumpForce;

			int healthVal;
			int damageVal;
			float speedVal;
			float jumpVal;
		};
	}
}