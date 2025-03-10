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

#include "../PS5Core/PS5Window.h"


//hi bow here, I have commented out anything to do with the terrain mesh because as of right now it is causing errors with the modular renderer

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
	inSelectionMode = false;

	allCoinsCollected=false;
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

	AssetManager::Instance().LoadAssets(&renderer);
	std::cout << "Assets loaded" << std::endl;

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


	/*if (testStateObject) {
		testStateObject->Update(dt);
	}*/
	

	if (player)
	{
		std::cout << "Player Update";
		player->Update(dt);
	}

	for (Enemy* enemy : enemies)
	{
		if (enemy)
		{
			enemy->Update(dt);
		}
	}

	if (doorTrigger)
	{
		doorTrigger->Update(dt);
	}

	CheckCoinsCollected();

	// if (!inSelectionMode) {
	// 	world->GetMainCamera().UpdateCamera(dt);
	// }
	
	UpdateKeys();

	if (useGravity) {
		//Debug::Print("(G)ravity on", Vector2(5, 95), Debug::RED);
	}
	else {
		//Debug::Print("(G)ravity off", Vector2(5, 95), Debug::RED);
	}	

	//Animation Test
	/*frameTime-=dt;
	while (frameTime<0.0f)
	{
		currentFrame = (currentFrame+1) % AssetManager::Instance().idle->GetFrameCount();
		frameTime +=1.0f/AssetManager::Instance().idle->GetFrameRate();
	}*/
	
	DisplayPathfinding();

	

	world.UpdateWorld(dt);
	//renderer.Update(dt);
#ifdef _WIN32



	if (networkManager == nullptr) {
		std::cout << "network manager is null" << std::endl;
	}
	else {
		networkManager->Update();
	}

#endif // WIN32


	physics.Update(dt);
	thirdPersonCam->Update(dt);
	
	//renderer.Render();
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
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F9)) {
		world.ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F10)) {
		world.ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F7)) {
		world.ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F8)) {
		world.ShuffleObjects(false);
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


	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world.GetMainCamera().BuildViewMatrix();
	Matrix4 camWorld	= Matrix::Inverse(view);

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis = Vector::Normalise(fwdAxis);

	if (Window::GetKeyboard()->KeyDown(KeyCodes::UP)) {
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::DOWN)) {
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::NEXT)) {
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0,-10,0));
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyCodes::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
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
	SceneManager::Instance().AddTerrain(&world, Vector3(0, -3, 0) + offset, Vector3(70, 2, 70));
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

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position,float radius,float inverseMass,const Vector3& initialVelocity) {
	GameObject* sphere = new GameObject();

	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform().SetScale(Vector3(radius, radius, radius));
	sphere->GetTransform().SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(),AssetManager::Instance().sphereMesh, AssetManager::Instance().basicTex, AssetManager::Instance().basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(),sphere->GetBoundingVolume()));
	sphere->SetName("Sphere");

	PhysicsObject* physicsObject = sphere->GetPhysicsObject();
	physicsObject->SetInverseMass(inverseMass);
	physicsObject->SetLinearVelocity(initialVelocity);

	physicsObject->InitSphereInertia();

	world.AddGameObject(sphere);
	
	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2.0f);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), AssetManager::Instance().cubeMesh, AssetManager::Instance().basicTex, AssetManager::Instance().basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));
	cube->SetName("Cube");

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();
	
	world.AddGameObject(cube);

	return cube;
}

void TutorialGame::InitDefaultFloor() {
	Vector3 offset(20,0,20);

	SceneManager::Instance().AddDefaultFloorToWorld(&world, Vector3(0,-3,0)+offset, Vector3(70,2,70));
	SceneManager::Instance().AddDefaultFloorToWorld(&world, Vector3(70,-3,0)+offset, Vector3(1,10,70));
	SceneManager::Instance().AddDefaultFloorToWorld(&world, Vector3(0,-3,-70)+offset, Vector3(70,10,1));
	SceneManager::Instance().AddDefaultFloorToWorld(&world, Vector3(0,-3,70)+offset, Vector3(70,10,1));
	SceneManager::Instance().AddDefaultFloorToWorld(&world, Vector3(-70,-3,0)+offset, Vector3(1,10,70));
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 10, Vector3(0,0,0));
		}
	}
	
	//AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
				
			}
			else {
				AddSphereToWorld(position, sphereRadius, Constants::SPHERE_DEFAULT_MASS, Vector3(0,0,0));
			}
		}
	}
}

/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::Left)) {
			
			
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(world.GetMainCamera());

			RayCollision closestCollision;
			if (world.Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyCodes::L)) {
			if (selectionObject) {
				if (lockedObject == selectionObject) {
					lockedObject = nullptr;
				}
				else {
					lockedObject = selectionObject;
				}
			}
		}
	}
	else {
		//Debug::Print("Press Q to change to select mode!", Vector2(5, 85));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	//Debug::Print("Click Force:" + std::to_string(forceMagnitude), Vector2(5, 90));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::Right)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(world.GetMainCamera());

		RayCollision closestCollision;
		if (world.Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
				Debug::DrawLine(world.GetMainCamera().GetPosition(),selectionObject->GetTransform().GetPosition(), Vector4(0,0,1,1), 10);
			}
		}
	}
}

void TutorialGame::TestLinearMotion() {
	AddSphereToWorld(Vector3(-5, 20, 0), 2.0f, Constants::SPHERE_DEFAULT_MASS, Vector3(0,10,1));
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
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(45,0,12),Vector3(6,1,1)));
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(70,0,12),Vector3(6,1,1)));
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(60,0,30),Vector3(8,1,3)));
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(45,0,50),Vector3(8,1,3)));
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(70,0,50),Vector3(3,1,3)));
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(65,0,70),Vector3(8,1,3)));
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(&world,Vector3(25,0,50),Vector3(2,1,4)));

	SetWallColour();
}

void TutorialGame::SetWallColour()
{
	for (size_t i = 0; i < floors.size(); ++i) {
		floors[i]->GetRenderObject()->SetColour(Vector4(0.7,0.7,0.7,1));
	}
}

void TutorialGame::CheckCoinsCollected() {
	for (GameObject* coin : catCoins) {
		if (coin->IsActive()) {
			return; // If there are uncollected coins, return immediately

		}
	}

	// If all coins have been collected
	if (!allCoinsCollected) {
		allCoinsCollected = true; // mark as completed
	}

	if (allCoinsCollected)
	{
		ShowSuccessMessage();
	}
}

void TutorialGame::ShowSuccessMessage() {
	Debug::Print("You collected all the cats! Score:" + std::to_string( player->score), Vector2(20,50),Vector4(1,1,0,1));
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