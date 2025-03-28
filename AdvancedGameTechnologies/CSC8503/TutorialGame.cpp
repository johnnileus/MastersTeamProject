#pragma once

#include "TutorialGame.h"
#include "AudioManager.h"
#include "Event.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame() : controller(*Window::GetWindow()->GetKeyboard(), *Window::GetWindow()->GetMouse()) {
	world		= new GameWorld();

#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
	renderer->Init();
	renderer->InitStructures();
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics		= new PhysicsSystem(*world);

	navGrid = nullptr;
	navMeshAgent = nullptr;

	controller.MapAxis(0, "Sidestep");
	controller.MapAxis(1, "UpDown");
	controller.MapAxis(2, "Forward");

	controller.MapAxis(3, "XLook");
	controller.MapAxis(4, "YLook");

	forceMagnitude	= 1.0f;
	useGravity		=  false;
	AssetManager::Instance().LoadAssets(renderer);

	sceneManager = new SceneManager();
	sceneManager->InitScenes();

	EnemyTestScene* listener = dynamic_cast<EnemyTestScene*>(sceneManager->scenes["EnemyTestScene"]);
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
	world->ClearAndErase();
	
	//delete individual enemies first
	physics->Clear();

	world->GetMainCamera().SetController(controller);
	world->GetMainCamera().SetNearPlane(0.1f);
	world->GetMainCamera().SetFarPlane(500.0f);

	thirdPersonCam = new ThirdPersonCamera(&world->GetMainCamera(), controller);
	if (thirdPersonCam)
	{
		thirdPersonCam->SetPitch(0.0f);
		thirdPersonCam->SetYaw(0.0f);
	}
	std::cout << "aa " << sceneManager->scenes.size() << std::endl;
	std::cout << sceneManager << std::endl;
	
	player = Player::Instantiate(world, thirdPersonCam, Vector3(0, 0, 0));
	player->SetScoreGoal(upScoreGoal);

	sceneManager->SwitchScene(name, world);

	InitItems();

	enemyFrameCount = 1;

	meleeEnemyFrameCount = 0;
	meleeEnemyFrameCountMax = meleeEnemyList.size();

	rangedEnemyFrameCount = 0;
	rangedEnemyFrameCountMax = rangedEnemyList.size();

	ghostEnemyFrameCount = 0;
	ghostEnemyFrameCountMax = ghostEnemyList.size();
	this->useGravity = true;
	physics->UseGravity(useGravity);

	AudioManager::GetInstance().PlayEvent("event:/Gaming Background Sound");
}



TutorialGame::~TutorialGame()	{

	AssetManager::Instance().Cleanup();

	delete physics;
	delete renderer;
	delete thirdPersonCam;
	delete world;
}


void TutorialGame::UpdateGame(float dt) {
	if (!gamePaused) { // if game is not paused :)
		if (player) { player->Update(dt); }
		if (doorTrigger) { doorTrigger->Update(dt); }

		for (Enemy* enemy : enemies) {
			if (enemy) { enemy->Update(dt); }
		}
		UpdateKeys();
		world->UpdateWorld(dt);

		if (networkManager == nullptr) {
			std::cout << "network manager is null" << std::endl;
		}
		else {
			networkManager->Update();
		}
		SceneManager::Instance().UpdateBullets(world, dt);



		physics->Update(dt);
		thirdPersonCam->Update(dt);

		renderer->Render();
		Debug::UpdateRenderables(dt);

		//Timer
		timerSecs += dt;
		if (timerSecs >= 60.0f) {
			timerMins += 1;
			timerSecs = 0;
		}
		//Debug::Print("Time:" + std::to_string(static_cast<int>(timerMins)) + ":" + std::to_string(static_cast<int>(timerSecs)), Vector2(80, 15));

		NewLevel();

	}
	else {
		if (player) { player->PausedUpdate(dt); }
		UpdateKeys();

		renderer->Render();
		Debug::UpdateRenderables(dt);
	}
  
	UpdateEnemies(dt);
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::U)) {
		std::cout << "starting server" << std::endl;
		networkManager->StartAsServer();
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::I)) {
		std::cout << "starting client" << std::endl;
		networkManager->StartAsClient();

	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::P)) {
		TogglePaused();
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::H)) {
		InitScene("default");
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::B)) {
		InitScene("default2");
	}

}

void TutorialGame::InitCamera() {
	world->GetMainCamera().SetNearPlane(0.1f);
	world->GetMainCamera().SetFarPlane(500.0f);
	
	if (thirdPersonCam)
	{
		thirdPersonCam->SetPitch(0.0f);
		thirdPersonCam->SetYaw(0.0f);
	}
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {


	Scene::CreateRopeGroup(world);
	
	player = Player::Instantiate(world,thirdPersonCam,Vector3(20,0,30));

	Scene::GenerateWall(world);

	InitCatCoins();
	
	InitTerrain();
	Scene::InitDefaultFloor(world);
	InitItems();

	world->PrintObjects();

}

void TutorialGame::InitTerrain() {
	Vector3 offset(20, 0, 20);
	Scene::AddTerrain(world, Vector3(0, -3, 0) + offset, Vector3(70, 2, 70));
}

void TutorialGame::InitCatCoins() {
	// add CatCoin to the list
	catCoins.push_back(CatCoin::Instantiate(world, Vector3(5, 0, 0)));
	catCoins.push_back(CatCoin::Instantiate(world, Vector3(20, 0, 20)));
	catCoins.push_back(CatCoin::Instantiate(world, Vector3(-10, 0, 25)));
	catCoins.push_back(CatCoin::Instantiate(world, Vector3(30, 0, 45)));
	catCoins.push_back(CatCoin::Instantiate(world, Vector3(25, 0, -10)));
	catCoins.push_back(CatCoin::Instantiate(world, Vector3(5, 0, 50)));

}

void TutorialGame::InitDefaultFloor() {
	Vector3 offset(0,0,0);

	Scene::AddDefaultFloorToWorld(world, Vector3(0,-3,0)+offset, Vector3(129,2, 129));
	Scene::AddDefaultFloorToWorld(world, Vector3(130,-3,0)+offset, Vector3(1,10, 130));
	Scene::AddDefaultFloorToWorld(world, Vector3(0,-3,-130)+offset, Vector3(130,10,1));
	Scene::AddDefaultFloorToWorld(world, Vector3(0,-3, 130)+offset, Vector3(130,10,1));
	Scene::AddDefaultFloorToWorld(world, Vector3(-130,-3,0)+offset, Vector3(1,10, 130));
}

void TutorialGame::CreateRopeGroup()
{
	Rope::AddRopeToWorld(world, Vector3(0,0,-5),Vector3(15,0,-5),0.7f);
	Rope::AddRopeToWorld(world, Vector3(0,0,10),Vector3(15,0,10),0.7f);
	Rope::AddRopeToWorld(world, Vector3(-10,0,30),Vector3(-10,0,40),0.7f);
	Rope::AddRopeToWorld(world, Vector3(-10,0,30),Vector3(-5,0,20),0.8f);
	Rope::AddRopeToWorld(world, Vector3(-5,0,50),Vector3(-10,0,40),0.8f);
	
}

void TutorialGame::DisplayPathfinding() {
	for (size_t i = 1; i < testNodes.size(); ++i) {
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		// Draw the path using green lines
		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}
}



void TutorialGame::SetWallColour()
{
	for (size_t i = 0; i < floors.size(); ++i) {
		floors[i]->GetRenderObject()->SetColour(Vector4(0.7,0.7,0.7,1));
	}
}

void TutorialGame::ReloadLevel() {
	// Clear the current level
	world->ClearAndErase();
	physics->Clear();

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
	meleeEnemyList.emplace_back(SceneManager::Instance().AddEnemyToWorld(world, this->navGrid, Vector3(10,3,10), 1.0f, 1.0f));
}

void TutorialGame::Test(int a) {
	std::cout << "aAAAaaAaAAa" << a << std::endl;
}

void TutorialGame::InitItems() {
	for (int i = 0; i < 20; i++) {
		//int x = 10;
		int xRand = (std::rand() % 221) - 110;
		int zRand = (std::rand() % 221) - 110;
		int uidRand = (std::rand() % 5) + 1;
		PassiveItem::Instantiate(world, player, Vector3(xRand, 0, zRand), uidRand);
	}
}

void TutorialGame::UpdateEnemies(float dt) {
	//frames 1,4,7 ect
	//if (enemyFrameCount % 3 == 0) {
	if (meleeEnemyFrameCount >= meleeEnemyFrameCountMax) {
		meleeEnemyFrameCount = 0;
	}
	//updates one enemy at a time
	if(meleeEnemyFrameCountMax > 0) {
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
	if (timerMins == 2) {
		timerMins = 0;
		levelCount++;
		InitScene("EnemyTestScene");
	}
	else if (player->GetScore() == player->GetScoreGoal()) {
		timerMins = 0;
		timerSecs = 0;
		levelCount++;
		upScoreGoal = player->GetScoreGoal() + 200;
		player->SetScoreGoal(upScoreGoal);
		InitScene("EnemyTestScene");
	}
	else if (Window::GetKeyboard()->KeyPressed(KeyCodes::V)) {
		InitScene("EnemyTestScene");
	}
}