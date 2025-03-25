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

			void SetBGMVolume(float volume);
			float GetBGMVolume()const;


			void PlaySound(const std::string& soundName);
			void PlayLoopingSound(const std::string& filename, FMOD::Channel** channel);
			void Update();

		private:
			AudioManager() = default;
			~AudioManager() = default;
			std::unordered_map<std::string, FMOD::Sound*> sounds;
			FMOD::System* system = nullptr;

			std::string GetMediaPath(const std::string& filename);

			float bgmVolume = 1.0f;  //100% sound
			FMOD::Channel* bgmChannel = nullptr;  //only save BGM channel
			SceKernelModule fmodLib;
			
		};
	}
}

