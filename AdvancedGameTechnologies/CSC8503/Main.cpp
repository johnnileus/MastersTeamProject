#include "Window.h"

#include "Debug.h"

#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

#include "GameServer.h"
#include "GameClient.h"

#include "NavigationGrid.h"
#include "NavigationMesh.h"

#include "TutorialGame.h"
#include "NetworkedGame.h"

#include "PushdownMachine.h"

#include "PushdownState.h"

#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"

using namespace NCL;
using namespace CSC8503;

#include <chrono>
#include <thread>
#include <sstream>

std::vector<Vector3> testNodes;

class TestPacketReceiver : public PacketReceiver {
public:
	TestPacketReceiver(std::string name) {
		this->name = name;
	}

	void ReceivePacket(int type, GamePacket* payload, int source) {
		if (type == String_Message) {
			StringPacket* realPacket = (StringPacket*)payload;

			std::string msg = realPacket->GetStringFromData();

			std::cout << name << " received message: " << msg << std::endl;
		}
	}

protected:
	std::string name;
};


void TestPathfinding() {
	NavigationGrid grid("TestGrid1.txt");

	NavigationPath outPath;

	Vector3 startPos(80, 0, 10);
	Vector3 endPos(80, 0, 80);

	bool found = grid.FindPath(startPos, endPos, outPath);

	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		testNodes.push_back(pos);
	}
}

void DisplayPathfinding() {
	for (int i = 1; i < testNodes.size(); ++i) {
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}
}


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

	Window*w = Window::CreateGameWindow(initInfo);

	if (!w->HasInitialised()) {
		return -1;
	}	

	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);
	//TestPathfinding();


	TutorialGame* g = new TutorialGame();
	w->GetTimer().GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
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
		//DisplayPathfinding();

		w->SetTitle("Go Marble Ball: " + std::to_string(1000.0f * dt));

		g->UpdateGame(dt);
	}
	Window::DestroyGameWindow();
}

void TestStateMachine() {
	StateMachine* testMachine = new StateMachine();
	int data = 0;

	// 定义状态A
	State* A = new State([&](float dt) -> void {
		std::cout << "I’m in state A!\n";
		data++;
	});

	// 定义状态B
	State* B = new State([&](float dt) -> void {
		std::cout << "I’m in state B!\n";
		data--;
	});

	// 定义状态A到状态B的转换
	StateTransition* stateAB = new StateTransition(A, B, [&]() -> bool {
		return data > 10; // 当data大于10时，转换到状态B
	});

	// 定义状态B到状态A的转换
	StateTransition* stateBA = new StateTransition(B, A, [&]() -> bool {
		return data < 0; // 当data小于0时，转换回状态A
	});

	// 将状态和转换添加到状态机中
	testMachine->AddState(A);
	testMachine->AddState(B);
	testMachine->AddTransition(stateAB);
	testMachine->AddTransition(stateBA);

	// 测试状态机
	for (int i = 0; i < 100; ++i) {
		testMachine->Update(1.0f); // 每帧更新状态机
	}

	delete testMachine; // 释放内存
}

void TestNetworking() {
	NetworkBase::Initialise();

	TestPacketReceiver serverReceiver("Server");
	TestPacketReceiver clientReceiver("Client");

	int port = NetworkBase::GetDefaultPort();

	GameServer* server = new GameServer(port, 1);
	GameClient* client = new GameClient();

	server->RegisterPacketHandler(String_Message, &serverReceiver);
	client->RegisterPacketHandler(String_Message, &clientReceiver);

	bool canConnect = client->Connect(127, 0, 0, 1, port);

	if (canConnect) {
		for (int i = 0; i < 100; ++i) {
			// 为服务器消息创建临时变量
			StringPacket serverPacket("Server says hello! " + std::to_string(i));
			server->SendGlobalPacket(serverPacket);

			// 为客户端消息创建临时变量
			StringPacket clientPacket("Client says hello! " + std::to_string(i));
			client->SendPacket(clientPacket);

			server->UpdateServer();
			client->UpdateClient();

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	} else {
		std::cout << "Failed to connect client to server!" << std::endl;
	}

	NetworkBase::Destroy();
}



