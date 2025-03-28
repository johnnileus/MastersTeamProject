#pragma once
#include "UIStateManager.h"

class GameUI {

public:
	GameUI();
	~GameUI();

	void RenderUI();
	void RenderAmmoCounter();
	void RenderCrosshair();
	void RenderHealthBar();
	void RenderScoreAndTimer();
	void RenderWeaponUI();
	void LoadingScreen(float progress);
	void RenderTexture(string fileName);	
	GLuint LoadTexture(const char* fileName);
	void InitTexture();

protected:
	//state manager obj
	UIStateManager uiState;

	int currentAmmo;
	int totalAmmo;

	int maxHealth = 100;
	int currentHealth = maxHealth;
	int selected_weapon = 0;
	float fade_alpha = 0.0f;
	float flash_alpha = 0.0f;
	float loadingProgress = 0.0f;
	int textVar = 0;
	//texture ids
	GLuint ak;
	GLuint burst;
	GLuint shotgun;
	GLuint bg;
	std::vector<std::string> loadScreenText = {"Loading Textures...",
												"Loading Assets...",							
												"Initializing Engine",
												"Compiling Shaders", 
												"Generating World",
												"Spawning Entities",
												"Syncing Data",
												"Rendering Scene", 
												"Updating systems",
												"Loading Post Processing...",
												"Loading Complete"};
};
