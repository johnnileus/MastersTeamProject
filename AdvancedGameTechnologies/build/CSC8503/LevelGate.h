#pragma once
#include <vector>
#include "GameObject.h"
#include "GameWorld.h"
#include "Vector.h"

namespace NCL {
	namespace CSC8503 {
		class LevelGate : public GameObject {
			LevelGate();
			~LevelGate();

			static LevelGate* Instantiate(GameWorld* world);
		};
	}
}
