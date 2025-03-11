#pragma once
#include <GameWorld.h>


namespace NCL {
	namespace CSC8503 {

		class Scene {
			void InitScene();
			void UpdateScene();
		protected:
			GameWorld* world;
		};
	
	
	
	}
}