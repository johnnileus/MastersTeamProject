#pragma once
#include "Window.h"

#include "Debug.h"

#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"


#include "NavigationGrid.h"
#include "NavigationMesh.h"

#include "TutorialGame.h"

#include "PushdownMachine.h"

#include "PushdownState.h"

#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"
#include "AudioManager.h"



using namespace NCL;
using namespace CSC8503;

#include <chrono>
#include <thread>
#include <sstream>


TutorialGame* g;

/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/
int main() {
	WindowInitialisation initInfo;
	initInfo.width		= 1280;
	initInfo.height		= 720;
	initInfo.windowTitle = "GO MARBLE BALL";
	/*initInfo.fullScreen =true;*/

	Window*w = Window::CreateGameWindow(initInfo);

	if (!w->HasInitialised()) {
		return -1;
	}	




	AudioManager& audio = AudioManager::GetInstance();
	if (!audio.Init()) {
		return -1;
	}
	
	audio.LoadBank("../../Assets/Sounds/Master.bank");
	audio.LoadBank("../../Assets/Sounds/Master.strings.bank");
	audio.LoadBank("../../Assets/Sounds/Background Sound.bank");
	audio.LoadBank("../../Assets/Sounds/Game Menu.bank");
	audio.LoadBank("../../Assets/Sounds/Player.bank");
	audio.LoadBank("../../Assets/Sounds/Enemy AI.bank");
	audio.LoadBank("../../Assets/Sounds/Weapon.bank");
	audio.LoadBank("../../Assets/Sounds/Environmental Effect.bank");
	


	g = new TutorialGame();
	w->GetTimer().GetTimeDeltaSeconds(); //Clear the timer so we don't get a large first dt!

	w->ShowOSPointer(!g->GetCursorLocked());
	w->LockMouseToWindow(g->GetCursorLocked());

	//main game loop
	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyCodes::ESCAPE)) {
		float dt = w->GetTimer().GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::T)) {
			w->SetWindowPosition(0, 0);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::O)) {
			g->ToggleCursor();
		}

		//DisplayPathfinding();

		w->SetTitle("Go Marble Ball: " + std::to_string(1000.0f * dt));

		g->UpdateGame(dt);

		audio.Update();
	}

	audio.Shutdown();


	Window::DestroyGameWindow();
}

void TestStateMachine() {
	StateMachine* testMachine = new StateMachine();
	int data = 0;

	// Define State A
	State* A = new State([&](float dt) -> void {
		std::cout << "I’m in state A!\n";
		data++;
	});

	// Define State B
	State* B = new State([&](float dt) -> void {
		std::cout << "I’m in state B!\n";
		data--;
	});

	// Define transition from State A to State B
	StateTransition* stateAB = new StateTransition(A, B, [&]() -> bool {
		return data > 10; // Transition to State B when data exceeds 10
	});

	// Define transition from State B to State A
	StateTransition* stateBA = new StateTransition(B, A, [&]() -> bool {
		return data < 0; // Transition back to State A when data is less than 0
	});

	// Add states and transitions to the state machine
	testMachine->AddState(A);
	testMachine->AddState(B);
	testMachine->AddTransition(stateAB);
	testMachine->AddTransition(stateBA);

	// Test the state machine
	for (int i = 0; i < 100; ++i) {
		testMachine->Update(1.0f); // Update the state machine every frame
	}

	delete testMachine; // Free memory
}







