#pragma once
#include "Window.h"
#ifdef USEAGC
#include "PS5Window.h"
#endif // USEAGC



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
#ifdef USEAGC
using namespace PS5;
#endif // USEAGC

using namespace CSC8503;

#ifdef USEAGC
size_t sceUserMainThreadStackSize = 2 * 1024 * 1024;
extern const char sceUserMainThreadName[] = "TeamProjectGameMain";
int sceUserMainThreadPriority = SCE_KERNEL_PRIO_FIFO_DEFAULT;
size_t sceLibcHeapSize = 256 * 1024 * 1024;
#endif // USEAGC

#include <chrono>
#include <thread>
#include <sstream>
#ifdef USEAGC
#include <GameTechAGCRenderer.h>
#endif // USEAGC
#include <AudioManager.h>


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
	//std::unique_ptr<Window>		w = std::make_unique<Window>("Hello!", 1920, 1080);
	std::unique_ptr<GameWorld>		world = std::make_unique<GameWorld>();
	std::unique_ptr<PhysicsSystem>	physics = std::make_unique<PhysicsSystem>(*world);
	
#ifdef USEAGC
	std::unique_ptr<PS5Window>		w = std::make_unique<PS5Window>("Hello!", 1920, 1080);
	GameTechAGCRenderer* renderer = new GameTechAGCRenderer(*world); 
	PS5Controller* c = w->GetController();
#else
	WindowInitialisation initInfo;
	initInfo.width = 1280;
	initInfo.height = 720;
	initInfo.windowTitle = "GO MARBLE BALL";
	Window* w = Window::CreateGameWindow(initInfo);
	GameTechRenderer* renderer = new GameTechRenderer(*world);
#endif 

	/*if (!w->HasInitialised()) {
		std::cout << "Window failed to initialise!" << std::endl;
		return -1;
	}*/	

#ifdef _WIN32
	std::unique_ptr<NetworkedGame> networkedGame = std::make_unique<NetworkedGame>(*world, *renderer, *physics);
#endif // WIN32
#ifdef USEAGC
	c->MapAxis(0, "LeftX");
	c->MapAxis(1, "LeftY");

	c->MapAxis(2, "RightX");
	c->MapAxis(3, "RightY");

	c->MapAxis(4, "DX");
	c->MapAxis(5, "DY");

	c->MapButton(0, "Triangle");
	c->MapButton(1, "Circle");
	c->MapButton(2, "Cross");
	c->MapButton(3, "Square");

	//These are the axis/button aliases the inbuilt camera class reads from:
	c->MapAxis(0, "XLook");
	c->MapAxis(1, "YLook");

	c->MapAxis(2, "Sidestep");
	c->MapAxis(3, "Forward");

	c->MapButton(0, "Up");
	c->MapButton(2, "Down");
	c->MapButton(4, "L2");
	c->MapButton(5, "R2");
	c->MapButton(6, "L1");
	c->MapButton(7, "R1");
	c->MapButton(8, "L3");
	c->MapButton(9, "R3");
#endif // USEAGC


	AudioManager& audio = AudioManager::GetInstance();
	if (!audio.Init()) {
		return -1;
	}


	audio.PlaySound("drumloop.wav");
	


	std::cout << "Creating TutorialGame instance" << std::endl;
	std::unique_ptr<TutorialGame> g = std::make_unique<TutorialGame>(*world, *renderer, *physics);

	w->GetTimer().GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
#ifdef USEAGC
	while (w->UpdateWindow() && !c->GetNamedButton("Triangle")) {
#else
	w->ShowOSPointer(!g->GetCursorLocked());
	w->LockMouseToWindow(g->GetCursorLocked());
	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyCodes::ESCAPE)) {
#endif // USEAGC
		float dt = w->GetTimer().GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		//if (Window::GetKeyboard()->KeyPressed(KeyCodes::PRIOR)) {
		//	w->ShowConsole(true);
		//}
		//if (Window::GetKeyboard()->KeyPressed(KeyCodes::NEXT)) {
		//	w->ShowConsole(false);
		//}

		//if (Window::GetKeyboard()->KeyPressed(KeyCodes::T)) {
		//	w->SetWindowPosition(0, 0);
		//}

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::O)) {
			g->ToggleCursor();
		}

		////DisplayPathfinding();

		//w->SetTitle("Go Marble Ball: " + std::to_string(1000.0f * dt));

		g->UpdateGame(dt);
		renderer->Update(dt);
		renderer->Render();
		Debug::UpdateRenderables(dt);
	}

	audio.Shutdown();


#ifdef _WIN32
	Window::DestroyGameWindow();
#endif // _WIN32
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







