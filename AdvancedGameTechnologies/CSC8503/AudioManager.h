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
			FMOD::Studio::EventInstance* PlayEvent(const std::string& eventName);
			void Update();

			void SetMasterVolume(int volume); 

			void PlayBGM(const std::string& eventName);
			void PauseBGM(bool paused);

		private:
			AudioManager() = default;
			~AudioManager() = default;
			
			FMOD::Studio::System* studioSystem = nullptr;
			std::unordered_map<std::string, FMOD::Studio::Bank*>banks;
			std::unordered_map<std::string, FMOD::Studio::EventInstance*>events;
			FMOD::Studio::EventInstance* bgmInstance = nullptr;
			bool bgmPlaying = false;

			std::string GetMediaPath(const std::string& filename);
		};
	}
}
