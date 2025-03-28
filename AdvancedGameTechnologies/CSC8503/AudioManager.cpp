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

	FMOD_RESULT result = FMOD::Studio::System::create(&studioSystem);
	if (result != FMOD_OK) {
		std::cout << "FMOD system creation failed:" << FMOD_ErrorString(result) << std::endl;
		return false;
	}

	result = studioSystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK) {
		std::cout << "FMOD Studio initialization failed:" << FMOD_ErrorString(result) << std::endl;
		return false;
	}

	std::cout << "FMOD Studio initialized successfully." << std::endl;
	return true;
}

void AudioManager::Shutdown() {
	for (auto& bank : banks) {
		bank.second->unload();
	}
	banks.clear();

	for (auto& event : events) {
		event.second->release();
	}
	events.clear();

	if (studioSystem) {
		studioSystem->release();
		studioSystem = nullptr;
		std::cout << "FMOD Studio system shut down." << std::endl;
	}
}

std::string AudioManager::GetMediaPath(const std::string& filename) {
	
	std::string fullPath = NCL::Assets::SOUNDSDIR + filename;
	if (fs::exists(fullPath)) 
	{
		return fullPath; 
	}
	else 
	{
		std::cout << "Error: Sound file not found at " << fullPath << std::endl;
		return "";  
	}
}

void AudioManager::LoadBank(const std::string& bankName)
{
	std::string fullPath = GetMediaPath(bankName);
	if (fullPath.empty())
	{
		return;
	}

	FMOD::Studio::Bank* bank = nullptr;
	FMOD_RESULT result = studioSystem->loadBankFile(fullPath.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
	if (result != FMOD_OK)
	{
		std::cout << "Failed to load bank: " << bankName << ":" << FMOD_ErrorString(result) << std::endl;
		return;
	}

	banks[bankName] = bank;
	std::cout << "Loaded bank:" << bankName << std::endl;
}


FMOD::Studio::EventInstance* AudioManager::PlayEvent(const std::string& eventName)
{
	auto it = events.find(eventName);
	FMOD::Studio::EventInstance* eventInstance = nullptr;

	if (it != events.end()) {
		eventInstance = it->second;
	}
	else {
		FMOD::Studio::EventDescription* eventDescription = nullptr;
		FMOD_RESULT result = studioSystem->getEvent(eventName.c_str(), &eventDescription);
		if (result != FMOD_OK) {
			std::cout << "Failed to find event: " << eventName << " - " << FMOD_ErrorString(result) << std::endl;
			return nullptr;
		}

		result = eventDescription->createInstance(&eventInstance);
		if (result != FMOD_OK) {
			std::cout << "Failed to create event instance: " << eventName << " - " << FMOD_ErrorString(result) << std::endl;
			return nullptr;
		}

		events[eventName] = eventInstance;
	}

	eventInstance->start();
	return eventInstance;
}

void AudioManager::SetMasterVolume(int volume) {
	float fVolume = std::clamp(volume / 100.0f, 0.0f, 1.0f);

	if (studioSystem) {
		FMOD::Studio::Bus* masterBus = nullptr;

		FMOD_RESULT result = studioSystem->getBus("bus:/", &masterBus);
		if (result == FMOD_OK && masterBus) {
			masterBus->setVolume(fVolume); // Set the volume of the master bus
			std::cout << "[AudioManager] Set master volume to " << fVolume << std::endl;
		}
		else {
			std::cout << "[AudioManager] Failed to get master bus: " << FMOD_ErrorString(result) << std::endl;
		}
	}
}

void AudioManager::PlayBGM(const std::string& eventName) {
	if (bgmPlaying) return;

	FMOD::Studio::EventDescription* eventDesc = nullptr;
	FMOD_RESULT result = studioSystem->getEvent(eventName.c_str(), &eventDesc);
	if (result != FMOD_OK) {
		std::cout << "Failed to find BGM event: " << eventName << std::endl;
		return;
	}

	result = eventDesc->createInstance(&bgmInstance);
	if (result != FMOD_OK) {
		std::cout << "Failed to create BGM instance!" << std::endl;
		return;
	}

	bgmInstance->start();
	bgmPlaying = true;
}

void AudioManager::PauseBGM(bool paused) {
	if (bgmInstance) {
		bgmInstance->setPaused(paused);
	}
}

void AudioManager::Update()
{
	if (studioSystem)
	{
		studioSystem->update();
	}
}