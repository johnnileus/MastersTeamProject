#pragma once
#include "UIStateManager.h"
class GameUI {

public:
	GameUI();
	~GameUI();

	void RenderUI();
protected:
	//state manager obj
	UIStateManager uiState;
};
