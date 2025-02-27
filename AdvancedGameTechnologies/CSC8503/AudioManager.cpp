#include "AudioManager.h"

using namespace NCL;
using namespace CSC8503;

FMOD::System* AudioManager::system = nullptr;

bool AudioManager::Init() {
	FMOD_RESULT result = FMOD::System_Create(&system);
	if (result != FMOD_OK) {
		std::cout << "FMOD system creation failed:" << FMOD_ErrorString(result) << std::endl; 
		return false;
	}

	result = system->init(512, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK) {
		std::cout << "FMOD system initialization failed:" << FMOD_ErrorString(result) << std::endl;
		return false;
	}

	std::cout << "FMOD initialized successfully." << std::endl;
	return true;
}

void AudioManager::Shutdown() {
	if (system) {
		system->release();
		system = nullptr;
		std::cout << "FMOD system shut dowm." << std::endl;
	}
}

FMOD::System* AudioManager::GetSystem() {
	return system;
}
