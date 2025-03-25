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
	fmodStudioLib = sceKernelLoadStartModule("/app0/sce_module/libfmodstudioL.prx", 0, nullptr, 0, nullptr, nullptr);
	fmodLib = sceKernelLoadStartModule("/app0/sce_module/libfmodL.prx", 0, nullptr, 0, nullptr, nullptr);
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



void AudioManager::Update()
{
	if (studioSystem)
	{
		studioSystem->update();
	}
}