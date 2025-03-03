#include "AudioManager.h"

using namespace NCL;
using namespace CSC8503;


AudioManager& AudioManager::GetInstance() {
	static AudioManager instance;
	return instance;
}



bool AudioManager::Init() {
	std::cout << "Attempting to initialize FMOD..." << std::endl;

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
		system->close();
		system->release();
		system = nullptr;
		std::cout << "FMOD system shut dowm." << std::endl;
	}
}

