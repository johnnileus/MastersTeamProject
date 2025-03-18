#include "AudioManager.h"
#include "Assets.h"

using namespace NCL;
using namespace CSC8503;
namespace fs = std::filesystem;


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
	for (auto& sound : sounds) {
		sound.second->release();
	}
	sounds.clear();

	if (system) {
		system->close();
		system->release();
		system = nullptr;
		std::cout << "FMOD system shut dowm." << std::endl;
	}
}

std::string AudioManager::GetMediaPath(const std::string& filename) {
	
	std::string fullPath = NCL::Assets::SOUNDSDIR + filename;
	if (fs::exists(fullPath)) {
		return fullPath; 
	}
	else {
		std::cout << "Error: Sound file not found at " << fullPath << std::endl;
		return "";  
	}
}

void AudioManager::PlaySound(const std::string& filename) {
	auto it = sounds.find(filename);
	FMOD::Channel* channel = nullptr;

	if (it != sounds.end()) {
		FMOD::Sound* sound = it->second;
		//FMOD::Channel* channel = nullptr;
		FMOD_RESULT result = system->playSound(sound, nullptr, false, &channel);
		if (result != FMOD_OK) {
			std::cout << "FMOD playSound failed: " << FMOD_ErrorString(result) << std::endl;
			return;
		}
	}
	else {
		std::string fullPath = GetMediaPath(filename);

		if (!fullPath.empty()) {
			FMOD::Sound* sound = nullptr;
			FMOD_RESULT result = system->createSound(fullPath.c_str(), FMOD_DEFAULT, 0, &sound);
			if (result != FMOD_OK) {
				std::cout << "FMOD createSound failed: " << FMOD_ErrorString(result) << std::endl;
				return;
			}

			sounds[filename] = sound;
			//FMOD::Channel* channel = nullptr;
			result = system->playSound(sound, nullptr, false, &channel);
			if (result != FMOD_OK) {
				std::cout << "FMOD playSound failed: " << FMOD_ErrorString(result) << std::endl;
				return;
			}
		}
	}
	//only control named "BGM.wav" channel
	if (filename == "BGM.wav") {
		bgmChannel = channel;
		if (bgmChannel) {
			bgmChannel->setVolume(bgmVolume); // apply BGM volume
		}
	}
}

void AudioManager::PlayLoopingSound(const std::string& filename, FMOD::Channel** channel) {
	auto it = sounds.find(filename);
	FMOD::Sound* sound = nullptr;

	if (it != sounds.end()) {
		sound = it->second;
	}
	else {
		std::string fullPath = GetMediaPath(filename);
		if (fullPath.empty()) return;


		system->createSound(fullPath.c_str(), FMOD_LOOP_NORMAL, 0, &sound);
		sounds[filename] = sound;
	}

	system->playSound(sound, nullptr, false, channel);
}


void AudioManager::SetBGMVolume(float newVolume) {
	bgmVolume = std::clamp(newVolume, 0.0f, 1.0f); // set realm
	if (bgmChannel) {
		bgmChannel->setVolume(bgmVolume);
	}
	else {
		std::cout << "bgmChannel is nullptr!" << std::endl;
	}
}

float AudioManager::GetBGMVolume() const {
	return bgmVolume;
}


void AudioManager::Update() {
	if (system) {
		system->update();
	}
}