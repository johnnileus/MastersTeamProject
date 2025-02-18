#include "TutorialGame.h"



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

	forceMagnitude	= 1.0f;
	useGravity		= false;
	inSelectionMode = false;

	allCoinsCollected=false;

	world->GetMainCamera().SetController(controller);

	controller.MapAxis(0, "Sidestep");
	controller.MapAxis(1, "UpDown");
	controller.MapAxis(2, "Forward");

	controller.MapAxis(3, "XLook");
	controller.MapAxis(4, "YLook");


	InitialiseAssets();
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	
	AssetManager::Instance().LoadAssets(renderer);
	
	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()	{

	AssetManager::Instance().Cleanup();

	delete physics;
	delete renderer;
	delete world;
}


void TutorialGame::UpdateGame(float dt) {


	if (testStateObject) {
		testStateObject->Update(dt);
	}

	if (player)
	{
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
	
	if (!inSelectionMode) {
		world->GetMainCamera().UpdateCamera(dt);
	}
	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix::View(camPos, objPos, Vector3(0,1,0));

		Matrix4 modelMat = Matrix::Inverse(temp);

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera().SetPosition(camPos);
		world->GetMainCamera().SetPitch(angles.x);
		world->GetMainCamera().SetYaw(angles.y);
	}

	UpdateKeys();

	if (useGravity) {
		//Debug::Print("(G)ravity on", Vector2(5, 95), Debug::RED);
	}
	else {
		//Debug::Print("(G)ravity off", Vector2(5, 95), Debug::RED);
	}
	//This year we can draw debug textures as well!
	Debug::DrawTex(*AssetManager::Instance().woodTex, Vector2(10, 10), Vector2(5, 5), Debug::WHITE);

	RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::K) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);

		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
		}
	}
	
	DisplayPathfinding();
	
	SelectObject();
	MoveSelectedObject();

	

	world->UpdateWorld(dt);
	//renderer->Update(dt);

	if (networkManager == nullptr) {
		std::cout << "network manager is null" << std::endl;
	}
	else {
		networkManager->Update();
	}


	physics->Update(dt);

	renderer->Render();
	Debug::UpdateRenderables(dt);
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
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F8)) {
		world->ShuffleObjects(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::U)) {
		std::cout << "starting server" << std::endl;
		networkManager->StartAsServer();
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::I)) {
		std::cout << "starting client" << std::endl;
		networkManager->StartAsClient();

	}



	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera().BuildViewMatrix();
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
	world->GetMainCamera().SetNearPlane(0.1f);
	world->GetMainCamera().SetFarPlane(500.0f);
	world->GetMainCamera().SetPitch(-15.0f);
	world->GetMainCamera().SetYaw(315.0f);
	world->GetMainCamera().SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();
	//BridgeConstraintTest();

	CreateRopeGroup();
	
	InitPlayer();

	GenerateWall();

	InitCatCoins();
	
	doorTrigger = Door::Instantiate(world,Vector3(15,0,25),Vector3(20,0,0),Quaternion(),Quaternion());
	
	Enemy::Instantiate(world,enemies,player,Vector3(50,0,0));

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
	
	SceneManager::Instance().AddCubeToWorld(world,Vector3(5,0,0),Vector3(1,1,1),1);
	
	world->PrintObjects();

}

void TutorialGame::InitPlayer()
{
	player = new Player();

	float meshSize		= 1.0f;
	float inverseMass	= 10.0f;
	
	SphereVolume* volume  = new SphereVolume(1.0f);

	player->SetBoundingVolume((CollisionVolume*)volume);

	player->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(Vector3(20,0,30));

	player->SetRenderObject(new RenderObject(&player->GetTransform(), AssetManager::Instance().sphereMesh, AssetManager::Instance().metalTex, AssetManager::Instance().basicShader));
	player->SetPhysicsObject(new PhysicsObject(&player->GetTransform(), player->GetBoundingVolume()));

	player->GetPhysicsObject()->SetInverseMass(inverseMass);
	player->GetPhysicsObject()->InitSphereInertia();
	player->playerObject=player;
	player->myWorld=world;
	player->Init();

	world->AddGameObject(player);
	LockCameraToObject(player->playerObject);
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

	PhysicsObject* physicsObject = sphere->GetPhysicsObject();
	physicsObject->SetInverseMass(inverseMass);
	physicsObject->SetLinearVelocity(initialVelocity);

	physicsObject->InitSphereInertia();

	world->AddGameObject(sphere);
	
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

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();
	
	world->AddGameObject(cube);

	return cube;
}

void TutorialGame::InitDefaultFloor() {
	Vector3 offset(20,0,20);
	SceneManager::Instance().AddDefaultFloorToWorld(world, Vector3(0,-3,0)+offset, Vector3(70,2,70));
	SceneManager::Instance().AddDefaultFloorToWorld(world, Vector3(70,-3,0)+offset, Vector3(1,10,70));
	SceneManager::Instance().AddDefaultFloorToWorld(world, Vector3(0,-3,-70)+offset, Vector3(70,10,1));
	SceneManager::Instance().AddDefaultFloorToWorld(world, Vector3(0,-3,70)+offset, Vector3(70,10,1));
	SceneManager::Instance().AddDefaultFloorToWorld(world, Vector3(-70,-3,0)+offset, Vector3(1,10,70));
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

			Ray ray = CollisionDetection::BuildRayFromMouse(world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
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
		Ray ray = CollisionDetection::BuildRayFromMouse(world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
				Debug::DrawLine(world->GetMainCamera().GetPosition(),selectionObject->GetTransform().GetPosition(), Vector4(0,0,1,1), 10);
			}
		}
	}
}

void TutorialGame::TestLinearMotion() {
	AddSphereToWorld(Vector3(-5, 20, 0), 2.0f, Constants::SPHERE_DEFAULT_MASS, Vector3(0,10,1));
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

void TutorialGame::GenerateWall()
{
	// add all walls to the list
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(world,Vector3(45,0,12),Vector3(6,1,1)));
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(world,Vector3(70,0,12),Vector3(6,1,1)));
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(world,Vector3(60,0,30),Vector3(8,1,3)));
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(world,Vector3(45,0,50),Vector3(8,1,3)));
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(world,Vector3(70,0,50),Vector3(3,1,3)));
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(world,Vector3(35,0,70),Vector3(9,1,3)));
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(world,Vector3(65,0,70),Vector3(8,1,3)));
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(world,Vector3(10,0,50),Vector3(4,1,4)));
	floors.push_back(SceneManager::Instance().AddDefaultFloorToWorld(world,Vector3(25,0,50),Vector3(2,1,4)));

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
	world->ClearAndErase();
	physics->Clear();

	// Reinitialize the world
	InitWorld();

	std::cout << "Level reloaded!" << std::endl;
}



void TutorialGame::BroadcastPosition(){

	Vector3 pos = player->GetTransform().GetPosition();
	Quaternion rot = player->GetTransform().GetOrientation();


	TransformPacket transform(pos, rot);

	networkManager->BroadcastPacket(transform);
}

void TutorialGame::UpdateTransformFromServer(Vector3 pos, Quaternion rot) {
	player->GetTransform().SetOrientation(rot);
	player->GetTransform().SetPosition(pos);
}