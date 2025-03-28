#pragma once
#include "ImGui/imgui.h"
#include "Inventory.h"
#include "InventoryUI.h"
#include "iostream"

enum UIState {
	MainMenu,
	Multiplayer,
	InGame,
	Paused,
	InventorySys,
	Settings,
	EndGame
};

class UIStateManager : public InventoryUI {
private:
	UIState currentState = UIState::MainMenu;
	bool isWindowOpen = true;
	bool isPaused = false;
	int audio = 100;
	bool endScreenFlag = false;

public:
	UIStateManager();
	~UIStateManager();

	void States();
	UIState GetCurrentState();
	void SetCurrentState(UIState state);
	void HandleInput();
};