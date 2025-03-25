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

protected:
	//state manager obj
	UIStateManager uiState;

	int currentAmmo;
	int totalAmmo;

	int maxHealth = 100;
	int currentHealth = maxHealth;

};
