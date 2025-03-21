#pragma once

#include "TutorialGame.h"
#include "AudioManager.h"

//#include "json/json11.hpp"
//#include <iostream>
//#include <fstream>
//#include <sstream>

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
	useGravity		= false;
	AssetManager::Instance().LoadAssets(renderer);

	sceneManager = new SceneManager();
	sceneManager->InitScenes();

	InitScene("default");
}

void TutorialGame::InitScene(string name) {
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
	
	player = Player::Instantiate(world, thirdPersonCam, Vector3(20, 0, 30));

	sceneManager->SwitchScene(name, world);
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
		while (timer >= 0.0f) {
			timer -= dt;
		}

		Debug::Print("Time:" + std::to_string(static_cast<int>(timer)), Vector2(80, 15));
		if (timer <= 0) {
			Transition();
		}

	}
	else {
		if (player) { player->PausedUpdate(dt); }
		UpdateKeys();

		renderer->Render();
		Debug::UpdateRenderables(dt);
	}

	DisplayPathfinding();
  
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

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F3)) {
		InitNavigationTestLevel(); //Loads a blank floor with navigation nodes displayed
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
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::V)) {
		InitScene("default");
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::B)) {
		InitScene("default2");
	}

	// BGM volume adjust
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM1)) { // botton 1 less volume
		bgmVolume -= 0.1f;
		if (bgmVolume < 0.0f) bgmVolume = 0.0f;
		AudioManager::GetInstance().SetBGMVolume(bgmVolume);
		std::cout << "BGM Volume Down: " << bgmVolume * 100 << "%" << std::endl;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM2)) { // botton 2 up volume
		bgmVolume += 0.1f;
		if (bgmVolume > 1.0f) bgmVolume = 1.0f;
		AudioManager::GetInstance().SetBGMVolume(bgmVolume);
		std::cout << "BGM Volume Up: " << bgmVolume * 100 << "%" << std::endl;
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
	
	//doorTrigger = Door::Instantiate(world,Vector3(15,0,25),Vector3(20,0,0),Quaternion(),Quaternion());
	

	InitEnemies();

	//InitTerrain();

	Scene::InitDefaultFloor(world);


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
	Vector3 offset(20,0,20);

	Scene::AddDefaultFloorToWorld(world, Vector3(0,-3,0)+offset, Vector3(70,2,70));
	Scene::AddDefaultFloorToWorld(world, Vector3(70,-3,0)+offset, Vector3(1,10,70));
	Scene::AddDefaultFloorToWorld(world, Vector3(0,-3,-70)+offset, Vector3(70,10,1));
	Scene::AddDefaultFloorToWorld(world, Vector3(0,-3,70)+offset, Vector3(70,10,1));
	Scene::AddDefaultFloorToWorld(world, Vector3(-70,-3,0)+offset, Vector3(1,10,70));
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

void TutorialGame::Transition() {
	return;
}

void TutorialGame::InitNavigationTestLevel() {
	//set camera to a debug camera

	//draw debug graph of all nodes and edges
	navGrid = new NavMeshGrid();
	navMeshAgent = new NavMeshAgent();
	std::vector<NavMeshNode> nodes = navGrid->GetAllNodes();
	for (int n = 0; n < nodes.size(); ++n) {
		Vector3 nodePos = nodes[n].GetPosition();
		Debug::DrawLine(Vector3(nodePos.x, nodePos.y - 2, nodePos.z), Vector3(nodePos.x, nodePos.y + 2, nodePos.z), Vector4(1,1,1,1), 60.0F);
		for (int e = 0; e < nodes[n].GetEdges().size(); ++e) {
			Debug::DrawLine(nodes[n].GetPosition(), nodes[n].GetEdges()[e].neighbour->GetPosition(), Vector4(0, 0, 1, 0.7), 60.0F);
		}
	}
	
	//run function to pick a start and end node, then highlight them in differnet colour
	//change the colour of each edge/node in the path to test it is working correctly
	//add a test function to randomly select some nodes as impassable then test again
}

void TutorialGame::ToggleCursor() {
	cursorLocked = !cursorLocked;
	Window::GetWindow()->ShowOSPointer(!cursorLocked);
	Window::GetWindow()->LockMouseToWindow(cursorLocked);
}

void TutorialGame::InitEnemies() {
	enemyList.emplace_back(SceneManager::Instance().AddEnemyToWorld(world, Vector3(10,3,10), 1.0f, 100.0f));
}

void TutorialGame::UpdateEnemies(float dt) {
	for (int e = 0; e < enemies.size(); ++e) {
		if (enemyList[e]->CheckAlive()) {
			//alive enemy logic
			continue;
		}
		else {
			if (enemyList[e]->CheckRespawn()) {
				enemyList[e]->Spawn();
			}
			else {
				enemyList[e]->UpdateRespawnTimer(dt);
			}
		}
	}
}