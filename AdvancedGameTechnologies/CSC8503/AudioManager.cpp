#include "AudioManager.h"

using namespace NCL;
using namespace CSC8503;


AudioManager& AudioManager::GetInstance() {
	static AudioManager instance;
	return instance;
}

// privet function
AudioManager::AudioManager() : system(nullptr) {}

// relsie source
AudioManager::~AudioManager() {
	Shutdown();
}

FMOD::System* AudioManager::system = nullptr;
FMOD::Channel* AudioManager::bgmChannel = nullptr;
std::unordered_map<std::string, FMOD::Sound*> AudioManager::sounds;
std::unordered_map<std::string, FMOD::Channel*> AudioManager::channels;

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
	// relese all sound resource
	for (auto& pair : sounds) {
		pair.second->release();
	}
	sounds.clear(); // clean harsh table
	if (system) {
		system->close();
		system->release();
		system = nullptr;
		std::cout << "FMOD system shut dowm." << std::endl;
	}
}

bool AudioManager::LoadSound(const std::string& name, const std::string& filePath, bool loop) {
	if (sounds.find(name) != sounds.end()) {
		std::cout << "Sound already loaded: " << name << std::endl;
		return true;
	}

	FMOD::Sound* sound;
	FMOD_MODE mode = loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT;
	FMOD_RESULT result = system->createSound(filePath.c_str(), mode, 0, &sound);

	if (result != FMOD_OK) {
		std::cout << "Failed to load sound: " << filePath << " - " << FMOD_ErrorString(result) << std::endl;
		return false;
	}

	sounds[name] = sound;
	std::cout << "Sound loaded: " << name << std::endl;
	return true;
}

void AudioManager::PlaySound(const std::string& name, bool loop = false) {
	auto it = sounds.find(name);
	if (it != sounds.end()) {
		system->playSound(it->second, nullptr, false, nullptr);
		std::cout << "Playing sound: " << name << std::endl;
	}
	else {
		std::cout << "Sound not found: " << name << std::endl;
	}
}

FMOD::System* AudioManager::GetSystem() {
	return system;
}
