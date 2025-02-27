#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>
#include <iostream>

namespace NCL {
	namespace CSC8503 {

		class AudioManager {
		public:
			static bool Init();
			static void Shutdown();
			static FMOD::System* GetSystem();

		private:
			static FMOD::System* system;
		};
	}
}

