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

			void Init();

			static PassiveItem* Instantiate(GameWorld* world, std::vector<PassiveItem*> itemList, Player* player, const Vector3& position, int uid);

			void OnCollisionBegin(GameObject* otherObject) override;

			GameWorld* myWorld;

			Player* myPlayer;
		protected:
			void SetComponent(float meshSize, float InverseMass);
			void SetUid(int uid);
			void RemoveObject(GameObject* object);
			void UpdateStats(Player* player, int uid);
			float size;
			float mass;
			int myUid;

			float maxSpeed;
			int health;
		};
	}
}