#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>
#include <iostream>
#include <string>
#include <unordered_map>

namespace NCL {
	namespace CSC8503 {

		class AudioManager {
		public:
			static AudioManager& GetInstance();

			bool Init();                   //initial FMOD
			void Shutdown();               //release FMOD
			

		private:
			AudioManager() : system(nullptr) {} 
			~AudioManager() = default;

			FMOD::System* system;
			
		};
	}
}

