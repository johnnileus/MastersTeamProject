#pragma once

#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <fmod_studio.h>
#include <filesystem>

namespace NCL {
	namespace CSC8503 {

		class AudioManager {
		public:
			static AudioManager& GetInstance();

			bool Init();                   //initial FMOD
			void Shutdown();               //release FMOD

			void LoadBank(const std::string& bankName);
			void PlayEvent(const std::string& eventName);
			void Update();

		private:
			AudioManager() = default;
			~AudioManager() = default;
			
			FMOD::Studio::System* studioSystem = nullptr;
			std::unordered_map<std::string, FMOD::Studio::Bank*>banks;
			std::unordered_map<std::string, FMOD::Studio::EventInstance*>events;

			std::string GetMediaPath(const std::string& filename);
		};
	}
}
