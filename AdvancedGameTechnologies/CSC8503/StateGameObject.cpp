#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject() {
	counter = 0.0f; // 初始化计数器
	stateMachine = new StateMachine();

	// 定义状态A：向左移动
	State* stateA = new State([&](float dt) -> void {
		this->MoveLeft(dt);
	});

	// 定义状态B：向右移动
	State* stateB = new State([&](float dt) -> void {
		this->MoveRight(dt);
	});

	// 添加状态到状态机
	stateMachine->AddState(stateA);
	stateMachine->AddState(stateB);

	// 定义从状态A到状态B的转换
	stateMachine->AddTransition(new StateTransition(stateA, stateB, [&]() -> bool {
		return this->counter > 3.0f; // 当计数器大于3时切换到状态B
	}));

	// 定义从状态B到状态A的转换
	stateMachine->AddTransition(new StateTransition(stateB, stateA, [&]() -> bool {
		return this->counter < 0.0f; // 当计数器小于0时切换到状态A
	}));
}

StateGameObject::~StateGameObject() {
	delete stateMachine;
}

void StateGameObject::Update(float dt) {
	stateMachine->Update(dt); // 更新状态机
}

void StateGameObject::MoveLeft(float dt) {
	GetPhysicsObject()->AddForce({ -100, 0, 0 }); // 向左施加力
	counter += dt; // 增加计数器值
}

void StateGameObject::MoveRight(float dt) {
	GetPhysicsObject()->AddForce({ 100, 0, 0 }); // 向右施加力
	counter -= dt; // 减少计数器值
}