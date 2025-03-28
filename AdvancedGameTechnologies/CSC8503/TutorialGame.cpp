#pragma once

#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include "AudioManager.h"
#include "Event.h"

#include "Pistol.h"
#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"
//#include "json/json11.hpp"
//#include <iostream>
//#include <fstream>
//#include <sstream>

#include "../PS5Core/PS5Window.h"



using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame(GameWorld& inWorld, GameTechRendererInterface& inRenderer, PhysicsSystem& inPhysics) : world(inWorld), renderer(inRenderer), physics(inPhysics),
#ifdef USEAGC
	controller(*((NCL::PS5::PS5Window*)Window::GetWindow())->GetController())
#else
	controller(*Window::GetWindow()->GetKeyboard(), *Window::GetWindow()->GetMouse())
#endif
{
	std::cout << "TutorialGame constructor called" << std::endl;
	//world		= new GameWorld();
#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
	renderer->Init();
	renderer->InitStructures();
#else 
	//renderer = new GameTechRenderer(*world);
#endif

	//physics		= new PhysicsSystem(*world);

	navGrid = nullptr;
	navMeshAgent = nullptr;

#ifdef _WIN32
	controller.MapAxis(0, "Sidestep");
	controller.MapAxis(1, "UpDown");
	controller.MapAxis(2, "Forward");

	controller.MapAxis(3, "XLook");
	controller.MapAxis(4, "YLook");
#endif // _WIN32

	forceMagnitude	= 1.0f;
	useGravity		=  false;
	AssetManager::Instance().LoadAssets(&renderer);

	sceneManager = new SceneManager();
	sceneManager->InitScenes();

#ifdef USEAGC
	EnemyTestScene* listener = static_cast<EnemyTestScene*>(sceneManager->scenes["EnemyTestScene"]);
#else
	EnemyTestScene* listener = dynamic_cast<EnemyTestScene*>(sceneManager->scenes["EnemyTestScene"]);
#endif // USEAGC
	listener->newMeleeEnemy.AddListener(std::bind(&TutorialGame::UpdateMeleeEnemyList, this, std::placeholders::_1));
	listener->newRangedEnemy.AddListener(std::bind(&TutorialGame::UpdateRangedEnemyList, this, std::placeholders::_1));
	listener->newGhostEnemy.AddListener(std::bind(&TutorialGame::UpdateGhostEnemyList, this, std::placeholders::_1));

	upScoreGoal = 300;

	InitScene("EnemyTestScene");
}

void TutorialGame::InitScene(string name) {
	this->useGravity = false;
	meleeEnemyList.clear();
	rangedEnemyList.clear();
	ghostEnemyList.clear();
	world.ClearAndErase();
	physics.Clear();
#ifdef USEAGC
	NCL::PS5::PS5Window* w = (NCL::PS5::PS5Window*)Window::GetWindow();
	NCL::PS5::PS5Controller* c = w->GetController();
	world.GetMainCamera().SetController(*c);
	world.GetMainCamera().SetNearPlane(0.1f);
	world.GetMainCamera().SetFarPlane(500.0f);

	thirdPersonCam = new ThirdPersonCamera(&world.GetMainCamera(), *c);
#else
	world.GetMainCamera().SetController(controller);
	world.GetMainCamera().SetNearPlane(0.1f);
	world.GetMainCamera().SetFarPlane(500.0f);

	thirdPersonCam = new ThirdPersonCamera(&world.GetMainCamera(), controller);
#endif // USEAGC
	if (thirdPersonCam)
	{
		thirdPersonCam->SetPitch(0.0f);
		thirdPersonCam->SetYaw(0.0f);
	}
	std::cout << "aa " << sceneManager->scenes.size() << std::endl;
	std::cout << sceneManager << std::endl;
#ifdef USEAGC
	player = Player::Instantiate(&world, thirdPersonCam, Vector3(0, 0, 0), *c);
#else
	player = Player::Instantiate(&world, thirdPersonCam, Vector3(0, 0, 0));
#endif // USEAGC
	player->SetScoreGoal(upScoreGoal);
	/*sceneManager->scenes[name]->InitScene(&world);*/

	sceneManager->SwitchScene(name, &world);

	InitItems();

	enemyFrameCount = 1;

	meleeEnemyFrameCount = 0;
	meleeEnemyFrameCountMax = meleeEnemyList.size();

	rangedEnemyFrameCount = 0;
	rangedEnemyFrameCountMax = rangedEnemyList.size();

	ghostEnemyFrameCount = 0;
	ghostEnemyFrameCountMax = ghostEnemyList.size();
	this->useGravity = true;
	physics.UseGravity(useGravity);
	AudioManager::GetInstance().PlayEvent("event:/Gaming Background Sound");
}




TutorialGame::~TutorialGame()	{

	AssetManager::Instance().Cleanup();

	//delete physics;
	//delete renderer;
	delete thirdPersonCam;
	//delete world;
}


void TutorialGame::UpdateGame(float dt) {
	//ammo counter should be running on all frames 
	if (mainMenu) {
#ifdef USEAGC
		NCL::PS5::PS5Window* w = (NCL::PS5::PS5Window*)Window::GetWindow();
		NCL::PS5::PS5Controller* c = w->GetController();
		world.GetMainCamera().SetController(*c);
#endif // USEAGC
		gamePaused = true;
		if (c->GetNamedButton("L2")) {
			//dummy function, this is required for some reason????
		}
		Debug::Print("Press Cross to Start Game", Vector2(40, 40), Vector4(0, 0, 1, 1));
		if (c->GetNamedButton("Cross")) {
			mainMenu = false;
			gamePaused = false;
		}
		Debug::Print("Press Triangle to Quit Game", Vector2(40, 70), Vector4(0, 0, 1, 1));
	}
	if (pauseGame && !mainMenu) {
#ifdef USEAGC
		NCL::PS5::PS5Window* w = (NCL::PS5::PS5Window*)Window::GetWindow();
		NCL::PS5::PS5Controller* c = w->GetController();
		world.GetMainCamera().SetController(*c);
#endif // USEAGC
		Debug::Print("Press Cross to Resume", Vector2(40, 40), Vector4(0, 0, 0, 1));
		if (c->GetNamedButton("Cross")) {
			gamePaused = false;
			pauseGame = false;
		}
		Debug::Print("Press Triangle to quit", Vector2(40, 80), Vector4(0, 0, 0, 1));
		//if (c->GetNamedButton("Circle")) {
		//	InitScene("EnemyTestScene");
		//	//ReloadLevel();
		//	pauseGame = false;
		//	mainMenu = true;
		//}
	}
	if (endGame) {
#ifdef USEAGC
		NCL::PS5::PS5Window* w = (NCL::PS5::PS5Window*)Window::GetWindow();
		NCL::PS5::PS5Controller* c = w->GetController();
		world.GetMainCamera().SetController(*c);
#endif // USEAGC
		//Debug::Print("Audio:" + to_string(score), Vector2(50, 50), Vector4(0, 1, 1, 1));
		Debug::Print("Press Triangle to Quit", Vector2(50, 60), Vector4(1, 0, 0, 1));
	}
	if (!gamePaused) { // if game is not paused :)
		if (player) { player->Update(dt); }
		if (doorTrigger) { doorTrigger->Update(dt); }

		for (Enemy* enemy : enemies) {
			if (enemy) { enemy->Update(dt); }
		}
		UpdateKeys();
		world.UpdateWorld(dt);

#ifdef _WIN32


		if (networkManager == nullptr) {
			std::cout << "network manager is null" << std::endl;
		}
		else {
			networkManager->Update();
		}
#endif // _WIN32


	SceneManager::Instance().UpdateBullets(&world, dt);
	//UpdateEnemies(dt);

		physics.Update(dt);
		thirdPersonCam->Update(dt);

		//renderer->Render();
		//Debug::UpdateRenderables(dt);

		//Timer
		timerSecs += dt;
		if (timerSecs >= 60.0f) {
			timerMins += 1;
			timerSecs = 0;
		}
		Debug::Print("Time:" + std::to_string(static_cast<int>(timerMins)) + ":" + std::to_string(static_cast<int>(timerSecs)), Vector2(80, 15));

		NewLevel();

	}
	else {
		if (player) { player->PausedUpdate(dt); }
		UpdateKeys();

		//renderer->Render();
		//Debug::UpdateRenderables(dt);


	}
  
	UpdateEnemies(dt);
}

void TutorialGame::UpdateKeys() {
#ifdef USEAGC
	NCL::PS5::PS5Window* w = (NCL::PS5::PS5Window*)Window::GetWindow();
	NCL::PS5::PS5Controller* c = w->GetController();
	world.GetMainCamera().SetController(*c);
#endif // USEAGC
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}
	
//#ifdef USEAGC
//	if (c->GetNamedButton("Square")) {
//#else
//	if (Window::GetKeyboard()->KeyPressed(KeyCodes::G)) {
//#endif // USEAGC
//
//		useGravity = !useGravity; //Toggle gravity!
//		physics.UseGravity(useGravity);
//	}

#ifdef _WIN32
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::U)) {
		std::cout << "starting server" << std::endl;
		networkManager->StartAsServer();
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::I)) {
		std::cout << "starting client" << std::endl;
		networkManager->StartAsClient();

	}
#endif // WIN32
#ifdef USEAGC
	if (c->GetNamedButton("Options")) {
#else
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::P)) {
#endif // USEAGC
		//TogglePaused();
		gamePaused = true;
		pauseGame = true;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::V)) {
		InitScene("default");
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::B)) {
		InitScene("default2");
	}
}

void TutorialGame::InitCamera() {
	world.GetMainCamera().SetNearPlane(0.1f);
	world.GetMainCamera().SetFarPlane(500.0f);
	
	if (thirdPersonCam)
	{
		thirdPersonCam->SetPitch(0.0f);
		thirdPersonCam->SetYaw(0.0f);
	}
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
#ifdef USEAGC
	NCL::PS5::PS5Window* w = (NCL::PS5::PS5Window*)Window::GetWindow();
	NCL::PS5::PS5Controller* c = w->GetController();
#endif // USEAGC


	Scene::CreateRopeGroup(&world);

#ifdef USEAGC
	player = Player::Instantiate(&world, thirdPersonCam, Vector3(20, 0, 30), *c);
#else
	player = Player::Instantiate(&world, thirdPersonCam, Vector3(20, 0, 30));
#endif // USEAGC

	Scene::GenerateWall(&world);

	InitCatCoins();

	InitTerrain();
	Scene::InitDefaultFloor(&world);
	InitItems();

	world.PrintObjects();
}

void TutorialGame::InitTerrain() {
	Vector3 offset(20, 0, 20);
	Scene::AddTerrain(&world, Vector3(0, -3, 0) + offset, Vector3(70, 2, 70));
}

void TutorialGame::InitCatCoins() {
	// add CatCoin to the list
	catCoins.push_back(CatCoin::Instantiate(&world, Vector3(5, 0, 0)));
	catCoins.push_back(CatCoin::Instantiate(&world, Vector3(20, 0, 20)));
	catCoins.push_back(CatCoin::Instantiate(&world, Vector3(-10, 0, 25)));
	catCoins.push_back(CatCoin::Instantiate(&world, Vector3(30, 0, 45)));
	catCoins.push_back(CatCoin::Instantiate(&world, Vector3(25, 0, -10)));
	catCoins.push_back(CatCoin::Instantiate(&world, Vector3(5, 0, 50)));

}

void TutorialGame::InitDefaultFloor() {
	Vector3 offset(0, 0, 0);

	Scene::AddDefaultFloorToWorld(&world, Vector3(0, -3, 0) + offset, Vector3(129, 2, 129));
	Scene::AddDefaultFloorToWorld(&world, Vector3(130, -3, 0) + offset, Vector3(1, 10, 130));
	Scene::AddDefaultFloorToWorld(&world, Vector3(0, -3, -130) + offset, Vector3(130, 10, 1));
	Scene::AddDefaultFloorToWorld(&world, Vector3(0, -3, 130) + offset, Vector3(130, 10, 1));
	Scene::AddDefaultFloorToWorld(&world, Vector3(-130, -3, 0) + offset, Vector3(1, 10, 130));
}




void TutorialGame::CreateRopeGroup()
{
	Rope::AddRopeToWorld(&world, Vector3(0, 0, -5), Vector3(15, 0, -5), 0.7f);
	Rope::AddRopeToWorld(&world, Vector3(0, 0, 10), Vector3(15, 0, 10), 0.7f);
	Rope::AddRopeToWorld(&world, Vector3(-10, 0, 30), Vector3(-10, 0, 40), 0.7f);
	Rope::AddRopeToWorld(&world, Vector3(-10, 0, 30), Vector3(-5, 0, 20), 0.8f);
	Rope::AddRopeToWorld(&world, Vector3(-5, 0, 50), Vector3(-10, 0, 40), 0.8f);

}



void TutorialGame::DisplayPathfinding() {
	for (size_t i = 1; i < testNodes.size(); ++i) {
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		// Draw the path using green lines
		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}
}

void TutorialGame::GenerateWall()
{
	// add all walls to the list
	//floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(45,0,12),Vector3(6,1,1)));
	//floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(70,0,12),Vector3(6,1,1)));
	//floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(60,0,30),Vector3(8,1,3)));
	//floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(45,0,50),Vector3(8,1,3)));
	//floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(70,0,50),Vector3(3,1,3)));
	//floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(35,0,70),Vector3(9,1,3)));
	//floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(65,0,70),Vector3(8,1,3)));
	//floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(10,0,50),Vector3(4,1,4)));
	//floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(25,0,50),Vector3(2,1,4)));

	SetWallColour();
}

void TutorialGame::SetWallColour()
{
	for (size_t i = 0; i < floors.size(); ++i) {
		floors[i]->GetRenderObject()->SetColour(Vector4(0.7, 0.7, 0.7, 1));
	}
}


void TutorialGame::ReloadLevel() {
	// Clear the current level
	world.ClearAndErase();
	physics.Clear();

	// Reinitialize the world
	InitWorld();

	std::cout << "Level reloaded!" << std::endl;
}

void TutorialGame::ToggleCursor() {
	cursorLocked = !cursorLocked;
	Window::GetWindow()->ShowOSPointer(!cursorLocked);
	Window::GetWindow()->LockMouseToWindow(cursorLocked);
}

void TutorialGame::InitEnemies() {
	meleeEnemyList.emplace_back(SceneManager::Instance().AddEnemyToWorld(&world, this->navGrid, Vector3(10, 3, 10), 1.0f, 1.0f));
}

void TutorialGame::InitItems() {
	for (int i = 0; i < 20; i++) {
		//int x = 10;
		int xRand = (std::rand() % 221) - 110;
		int zRand = (std::rand() % 221) - 110;
		int uidRand = (std::rand() % 5) + 1;
		PassiveItem::Instantiate(&world, player, Vector3(xRand, 0, zRand), uidRand);
	}
}

void TutorialGame::UpdateEnemies(float dt) {
	//frames 1,4,7 ect
	//if (enemyFrameCount % 3 == 0) {
	if (meleeEnemyFrameCount >= meleeEnemyFrameCountMax) {
		meleeEnemyFrameCount = 0;
	}
	//updates one enemy at a time
	if (meleeEnemyFrameCountMax > 0) {
		if (meleeEnemyList[meleeEnemyFrameCount]->CheckAlive()) {
			meleeEnemyList[meleeEnemyFrameCount]->UpdateEnemy(dt * meleeEnemyFrameCountMax);
		}
		else {
			if (meleeEnemyList[meleeEnemyFrameCount]->CheckRespawn()) {
				meleeEnemyList[meleeEnemyFrameCount]->Spawn();
				meleeEnemyList[meleeEnemyFrameCount]->SetDestinationNull();
			}
			else {
				meleeEnemyList[meleeEnemyFrameCount]->UpdateRespawnTimer(dt * meleeEnemyFrameCountMax);
			}
		}
	}
	meleeEnemyFrameCount++;
	//}

	//if (enemyFrameCount % 3 == 1) {
	if (rangedEnemyFrameCount >= rangedEnemyFrameCountMax) {
		rangedEnemyFrameCount = 0;
	}
	if (rangedEnemyFrameCountMax > 0) {
		if (rangedEnemyList[rangedEnemyFrameCount]->CheckAlive()) {
			rangedEnemyList[rangedEnemyFrameCount]->UpdateEnemy(dt * rangedEnemyFrameCountMax);
		}
		else {
			if (rangedEnemyList[rangedEnemyFrameCount]->CheckRespawn()) {
				rangedEnemyList[rangedEnemyFrameCount]->Spawn();
				rangedEnemyList[rangedEnemyFrameCount]->SetDestinationNull();
			}
			else {
				rangedEnemyList[rangedEnemyFrameCount]->UpdateRespawnTimer(dt * rangedEnemyFrameCountMax);
			}
		}
	}
	rangedEnemyFrameCount++;
	//}

	//if (enemyFrameCount % 3 == 2) {
	if (ghostEnemyFrameCount >= ghostEnemyFrameCountMax) {
		ghostEnemyFrameCount = 0;
	}
	if (ghostEnemyFrameCountMax > 0) {
		if (ghostEnemyList[ghostEnemyFrameCount]->CheckAlive()) {
			ghostEnemyList[ghostEnemyFrameCount]->UpdateEnemy(dt * ghostEnemyFrameCountMax);
		}
		else {
			if (ghostEnemyList[ghostEnemyFrameCount]->CheckRespawn()) {
				ghostEnemyList[ghostEnemyFrameCount]->Spawn();
				ghostEnemyList[ghostEnemyFrameCount]->SetDestinationNull();
			}
			else {
				ghostEnemyList[ghostEnemyFrameCount]->UpdateRespawnTimer(dt * ghostEnemyFrameCountMax);
			}
		}
	}
	ghostEnemyFrameCount++;
	//}

	enemyFrameCount++;
	if (enemyFrameCount == 4) {
		enemyFrameCount = 1;
	}
}

void TutorialGame::InitNavGrid() {
}

void TutorialGame::NewLevel() {
	if (timerMins == 2 && (!mainMenu || !pauseGame)) {
		timerMins = 0;
		levelCount++;
		InitScene("EnemyTestScene");
	}
	else if (player->GetScore() == player->GetScoreGoal() && (!mainMenu || !pauseGame)) {
		timerMins = 0;
		timerSecs = 0;
		levelCount++;
		upScoreGoal = player->GetScoreGoal() + 200;
		player->SetScoreGoal(upScoreGoal);
		InitScene("EnemyTestScene");
	}
}