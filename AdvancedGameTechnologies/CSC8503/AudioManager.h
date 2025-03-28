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
			void SetMasterVolume(int volume);
			void Update();


		private:
			AudioManager() = default;
			~AudioManager() = default;
			
			FMOD::Studio::System* studioSystem = nullptr;
			std::unordered_map<std::string, FMOD::Studio::Bank*>banks;
			std::unordered_map<std::string, FMOD::Studio::EventInstance*>events;
			FMOD::Studio::EventInstance* bgmInstance = nullptr;
			bool bgmPlaying = false;

			std::string GetMediaPath(const std::string& filename);

			//float bgmVolume = 1.0f;  //100% sound
			//FMOD::Channel* bgmChannel = nullptr;  //only save BGM channel
#ifdef USEAGC
			SceKernelModule fmodLib;
			SceKernelModule fmodStudioLib;
#endif // USEAGC	
			
		};
	}
}
