#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject() {
	counter = 0.0f; // Initialize counter
	stateMachine = new StateMachine();

	// Define State A: Move left
	State* stateA = new State([&](float dt) -> void {
		this->MoveLeft(dt);
	});

	// Define State B: Move right
	State* stateB = new State([&](float dt) -> void {
		this->MoveRight(dt);
	});

	// Add states to the state machine
	stateMachine->AddState(stateA);
	stateMachine->AddState(stateB);

	// Define transition from State A to State B
	stateMachine->AddTransition(new StateTransition(stateA, stateB, [&]() -> bool {
		return this->counter > 3.0f; // Switch to State B when the counter exceeds 3
	}));

	// Define transition from State B to State A
	stateMachine->AddTransition(new StateTransition(stateB, stateA, [&]() -> bool {
		return this->counter < 0.0f; // Switch to State A when the counter is below 0
	}));
}

StateGameObject::~StateGameObject() {
	delete stateMachine;
}

void StateGameObject::Update(float dt) {
	stateMachine->Update(dt); // Update state machine
}

void StateGameObject::MoveLeft(float dt) {
	GetPhysicsObject()->AddForce({ -100, 0, 0 }); // Apply force to move left
	counter += dt; // Increase counter
}

void StateGameObject::MoveRight(float dt) {
	GetPhysicsObject()->AddForce({ 100, 0, 0 }); // Apply force to move right
	counter -= dt; // Decrease counter
}
