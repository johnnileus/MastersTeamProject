#pragma once
#include "UIStateManager.h"
class GameUI {

public:
	GameUI();
	~GameUI();

	void RenderUI();
	void AmmoCounter();
	void Crosshair();
protected:
	//state manager obj
	UIStateManager uiState;
};
