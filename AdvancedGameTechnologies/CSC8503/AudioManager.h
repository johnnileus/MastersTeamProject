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
			FMOD::System* GetSystem();     //update FMOD

			bool LoadSound(const std::string& name, const std::string& filePath, bool loop = false);
			void PlaySound(const std::string& name, bool loop = false);  
			void PlayBGM(const std::string& name, bool loop = true);     
			void StopSound(const std::string& name);                    
			void StopBGM();                                              
			void SetVolume(const std::string& name, float volume);       

			void Set3DAttributes(const std::string& name, float x, float y, float z); 

		private:
			
			AudioManager();
			~AudioManager();
			AudioManager(const AudioManager&) = delete;
			AudioManager& operator=(const AudioManager&) = delete;

			static FMOD::System* system;
			static FMOD::Channel* bgmChannel;
			static std::unordered_map<std::string, FMOD::Sound*> sounds;
			static std::unordered_map<std::string, FMOD::Channel*> channels;
		};
	}
}

