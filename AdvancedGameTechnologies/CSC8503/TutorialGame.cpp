#pragma once

#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"

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

	

	forceMagnitude	= 1.0f;
	useGravity		= false;

#ifdef USEAGC
	NCL::PS5::PS5Window* w = (NCL::PS5::PS5Window*)Window::GetWindow();
	NCL::PS5::PS5Controller* c = w->GetController();
	world.GetMainCamera().SetController(*c);

      
	//c->MapAxis(0, "LeftX");
	//c->MapAxis(1, "LeftY");

	//c->MapAxis(2, "RightX");
	//c->MapAxis(3, "RightY");

	//c->MapAxis(4, "DX");
	//c->MapAxis(5, "DY");

	//c->MapButton(0, "Triangle");
	//c->MapButton(1, "Circle");
	//c->MapButton(2, "Cross");
	//c->MapButton(3, "Square");

	////These are the axis/button aliases the inbuilt camera class reads from:
	//c->MapAxis(0, "XLook");
	//c->MapAxis(1, "YLook");

	//c->MapAxis(2, "Sidestep");
	//c->MapAxis(3, "Forward");

	//c->MapButton(0, "Up");
	//c->MapButton(2, "Down");
	std::cout << "Creating ThirdPersonCamera instance" << std::endl;
	thirdPersonCam = new ThirdPersonCamera(&world.GetMainCamera(), *c);
	std::cout << "ThirdPersonCamera instance created" << std::endl;
#else
	world.GetMainCamera().SetController(controller);

	controller.MapAxis(0, "Sidestep");
	controller.MapAxis(1, "UpDown");
	controller.MapAxis(2, "Forward");

	controller.MapAxis(3, "XLook");
	controller.MapAxis(4, "YLook");
#endif // USEAGC
#ifdef _WIN32
	std::cout << "Creating ThirdPersonCamera instance" << std::endl;
	thirdPersonCam = new ThirdPersonCamera(&world.GetMainCamera(), controller);
	std::cout << "ThirdPersonCamera instance created" << std::endl;
#endif // _WIN32


	navGrid = nullptr;
	navMeshAgent = nullptr;
	
//#ifdef _WIN32
//	world.GetMainCamera().SetController(*w->GetController());
//#endif // _WIN32

	sceneManager = new SceneManager();

	InitialiseAssets();
	
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	std::cout << "InitialiseAssets called" << std::endl;
	//cubeMesh = renderer.LoadMesh("cube.msh");

	/*
	// DEBUG CODE
	std::ifstream file("items.json");
	if (!file) {
		std::cout << "Could not load json!" << std::endl;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	std::string err;
	json11::Json json = json11::Json::parse(buffer.str(), err);
	std::cout << "JSON data: " << json.dump() << "\n";
	*/

	AssetManager::Instance().LoadAssets(&renderer);
	
	InitCamera();
	std::cout << "Camera initialized" << std::endl;

	InitWorld();
	std::cout << "World initialized" << std::endl;
}

TutorialGame::~TutorialGame()	{

	AssetManager::Instance().Cleanup();

	//delete physics;
	//delete renderer;
	delete thirdPersonCam;
	//delete world;
}


void TutorialGame::UpdateGame(float dt) {
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
		while (timer >= 0.0f) {
			timer -= dt;
		}

		Debug::Print("Time:" + std::to_string(static_cast<int>(timer)), Vector2(80, 15));
		if (timer <= 0) {
			Transition();
		}

	}
	else {
		if (player) { player->PausedUpdate(dt); } //HI BOW HERE, THIS WILL NOT WORK IN THIS BUILD AS RENDERER IS NOT UPDATED HERE, NEED TO TRANSFER THIS TO MAIN SOMEHOW, POSSIBLY A FLAG?
		UpdateKeys();

		//renderer->Render();
		//Debug::UpdateRenderables(dt);


	}

	DisplayPathfinding();
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
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F3)) {
		InitNavigationTestLevel(); //Loads a blank floor with navigation nodes displayed
	}
#ifdef USEAGC
	if (c->GetNamedButton("Square")) {
#else
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::G)) {
#endif // USEAGC

		useGravity = !useGravity; //Toggle gravity!
		physics.UseGravity(useGravity);
	}

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
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::P)) {
		TogglePaused();
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
	world.ClearAndErase();
	physics.Clear();
#ifdef USEAGC
	NCL::PS5::PS5Window* w = (NCL::PS5::PS5Window*)Window::GetWindow();
	NCL::PS5::PS5Controller* c = w->GetController();
#endif // USEAGC

	

	CreateRopeGroup();
	
	//InitPlayer();
#ifdef USEAGC
	player = Player::Instantiate(&world, thirdPersonCam, Vector3(20, 0, 30), *c);
#else
	player = Player::Instantiate(&world, thirdPersonCam, Vector3(20, 0, 30));
#endif // USEAGC
	GenerateWall();

	InitCatCoins();
	
	doorTrigger = Door::Instantiate(&world,Vector3(15,0,25),Vector3(20,0,0),Quaternion(),Quaternion());
	
	Enemy::Instantiate(&world,enemies,player,Vector3(50,0,0));

	InitEnemies();

	//InitTerrain();

	InitDefaultFloor();

	// Load the navigation grid
	NavigationGrid* navGrid = new NavigationGrid("TestGrid1.txt");

	// Generate a test path
	NavigationPath outPath;
	Vector3 startPos(80, 0, 10); // Start position
	Vector3 endPos(80, 0, 80);   // End position

	if (navGrid->FindPath(startPos, endPos, outPath)) {
		Vector3 pos;
		while (outPath.PopWaypoint(pos)) {
			testNodes.push_back(pos); // Store path waypoints
		}

		enemies[0]->SetMovePath(testNodes);
	}
	
	SceneManager::Instance().AddCubeToWorld(&world,Vector3(5,0,0),Vector3(1,1,1),1);
	
	world.PrintObjects();

}

// if modifying the shape, please change InitialiseConnectedPlayer as well
//void TutorialGame::InitPlayer()
//{
//	player = new Player();
//
//	float meshSize		= 1.0f;
//	float inverseMass	= 10.0f;
//	
//	SphereVolume* volume  = new SphereVolume(1.0f);
//
//	player->SetBoundingVolume((CollisionVolume*)volume);
//
//	player->GetTransform()
//		.SetScale(Vector3(meshSize, meshSize, meshSize))
//		.SetPosition(Vector3(20,0,30));
//
//	player->SetRenderObject(new RenderObject(&player->GetTransform(), AssetManager::Instance().sphereMesh, AssetManager::Instance().metalTex, AssetManager::Instance().basicShader));
//	player->SetPhysicsObject(new PhysicsObject(&player->GetTransform(), player->GetBoundingVolume()));
//
//	player->GetPhysicsObject()->SetInverseMass(inverseMass);
//	player->GetPhysicsObject()->InitSphereInertia();
//	player->playerObject=player;
//	player->myWorld=world;
//	player->Init(thirdPersonCam);
//
//	world->AddGameObject(player);
//
//	if (thirdPersonCam)
//	{
//		thirdPersonCam->SetFollowObject(player);
//	}
//	
//}

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
	Vector3 offset(20,0,20);

	Scene::AddDefaultFloorToWorld(&world, Vector3(0,-3,0)+offset, Vector3(70,2,70));
	Scene::AddDefaultFloorToWorld(&world, Vector3(70,-3,0)+offset, Vector3(1,10,70));
	Scene::AddDefaultFloorToWorld(&world, Vector3(0,-3,-70)+offset, Vector3(70,10,1));
	Scene::AddDefaultFloorToWorld(&world, Vector3(0,-3,70)+offset, Vector3(70,10,1));
	Scene::AddDefaultFloorToWorld(&world, Vector3(-70,-3,0)+offset, Vector3(1,10,70));
}




void TutorialGame::CreateRopeGroup()
{
	Rope::AddRopeToWorld(&world, Vector3(0,0,-5),Vector3(15,0,-5),0.7f);
	Rope::AddRopeToWorld(&world, Vector3(0,0,10),Vector3(15,0,10),0.7f);
	Rope::AddRopeToWorld(&world, Vector3(-10,0,30),Vector3(-10,0,40),0.7f);
	Rope::AddRopeToWorld(&world, Vector3(-10,0,30),Vector3(-5,0,20),0.8f);
	Rope::AddRopeToWorld(&world, Vector3(-5,0,50),Vector3(-10,0,40),0.8f);
	
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
		floors[i]->GetRenderObject()->SetColour(Vector4(0.7,0.7,0.7,1));
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
	/*enemyList.emplace_back(SceneManager::Instance().AddEnemyToWorld(&world, Vector3(10,3,10), 1.0f, 100.0f));*/
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