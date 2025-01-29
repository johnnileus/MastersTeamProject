#include "TutorialGame.h"

#include "CatCoin.h"

#include "Constants.h"
#include "Door.h"
#include "Enemy.h"
#include "GameWorld.h"
#include "NavigationGrid.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "Player.h"
#include "StateGameObject.h"

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
	cubeMesh	= renderer->LoadMesh("cube.msh");
	sphereMesh	= renderer->LoadMesh("sphere.msh");
	catMesh		= renderer->LoadMesh("ORIGAMI_Chat.msh");
	kittenMesh	= renderer->LoadMesh("Kitten.msh");

	enemyMesh	= renderer->LoadMesh("Keeper.msh");
	bonusMesh	= renderer->LoadMesh("19463_Kitten_Head_v1.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");

	basicTex	= renderer->LoadTexture("checkerboard.png");
	woodTex     = renderer->LoadTexture("wood.png");
	metalTex	= renderer->LoadTexture("Metal.png");
	tilesTex	= renderer->LoadTexture("Tiles.png");
	floorTex	= renderer->LoadTexture("WoodFloor.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");
	
	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete catMesh;
	delete kittenMesh;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

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
	//Debug::DrawTex(*basicTex, Vector2(10, 10), Vector2(5, 5), Debug::MAGENTA);

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

	doorTrigger = CreateDoor(Vector3(15,0,25));

	AddEnemyToPoision(Vector3(50,0,0));
	AddCatCoinToPosition(Vector3(5,0,0));
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

	player->SetRenderObject(new RenderObject(&player->GetTransform(), sphereMesh, metalTex, basicShader));
	player->SetPhysicsObject(new PhysicsObject(&player->GetTransform(), player->GetBoundingVolume()));

	player->GetPhysicsObject()->SetInverseMass(inverseMass);
	player->GetPhysicsObject()->InitSphereInertia();
	player->playerObject=player;
	player->myWorld=world;
	player->Init();

	world->AddGameObject(player);
	LockCameraToObject(player->playerObject);
}


/*

A single function to add a large immoveable cube to the bottom of our world
*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position,const Vector3& size) {
	GameObject* floor = new GameObject();
	floor->tag = "Ground";
	floor->SetName("floor");
	//Vector3 floorSize = Vector3(70, 2, 70);
	Vector3 floorSize = size;
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2.0f)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, floorTex, basicShader));
	floor->GetRenderObject()->SetColour(Vector4(1,1,1,1));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);
	
	return floor;
}

void TutorialGame::InitCatCoins() {
	// 添加 CatCoin 到列表
	catCoins.push_back(AddCatCoinToPosition(Vector3(5, 0, 0)));
	catCoins.push_back(AddCatCoinToPosition(Vector3(20, 0, 20)));
	catCoins.push_back(AddCatCoinToPosition(Vector3(-10, 0, 25)));
	catCoins.push_back(AddCatCoinToPosition(Vector3(30, 0, 45)));
	catCoins.push_back(AddCatCoinToPosition(Vector3(25, 0, -10)));
	catCoins.push_back(AddCatCoinToPosition(Vector3(5, 0, 50)));
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

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(),sphereMesh, basicTex, basicShader));
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

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();
	
	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize		= 1.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();
	SphereVolume* volume  = new SphereVolume(1.0f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), catMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 3.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

void TutorialGame::AddEnemyToPoision(const Vector3& posision)
{
	Enemy* enemy = new Enemy(player,world);
	float meshSize		= 1.0f;
	float inverseMass	= 25.0f;
	SphereVolume* volume  = new SphereVolume(1.0f);

	enemy->SetBoundingVolume((CollisionVolume*)volume);

	enemy->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(posision);

	enemy->SetRenderObject(new RenderObject(&enemy->GetTransform(), sphereMesh, woodTex, basicShader));
	enemy->SetPhysicsObject(new PhysicsObject(&enemy->GetTransform(), enemy->GetBoundingVolume()));

	enemy->GetPhysicsObject()->SetInverseMass(inverseMass);
	enemy->GetPhysicsObject()->InitSphereInertia();
	enemy->Init();
	enemies.push_back(enemy);
	
	world->AddGameObject(enemy);
	
}

GameObject* TutorialGame::AddCatCoinToPosition(const Vector3& posision)
{
	CatCoin* coin = new CatCoin();
	float meshSize		= 2.0f;
	float inverseMass	= 25.0f;
	SphereVolume* volume  = new SphereVolume(1.0f);

	coin->SetBoundingVolume((CollisionVolume*)volume);

	coin->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(posision);

	coin->SetRenderObject(new RenderObject(&coin->GetTransform(), bonusMesh, nullptr, basicShader));
	coin->SetPhysicsObject(new PhysicsObject(&coin->GetTransform(), coin->GetBoundingVolume()));

	coin->GetPhysicsObject()->SetInverseMass(inverseMass);
	coin->GetPhysicsObject()->InitSphereInertia();
	coin->coinGameObject=coin;
	
	coin->Init();
	
	world->AddGameObject(coin);

	return  coin;
}



GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(1.5, 1.5, 1.5))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));
	apple->GetRenderObject()->SetColour(Vector4(1,0.992,0.467,1));
	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

void TutorialGame::InitDefaultFloor() {
	Vector3 offset(20,0,20);
	AddFloorToWorld(Vector3(0, -3, 0)+offset, Vector3(70,2,70));//down
	AddFloorToWorld(Vector3(70,-3,0)+offset, Vector3(1,10,70));//right
	AddFloorToWorld(Vector3(0, -3, -70)+offset, Vector3(70, 10, 1));//front
	AddFloorToWorld(Vector3(0, -3, 70)+offset, Vector3(70, 10, 1));//back
	AddFloorToWorld(Vector3(-70, -3, 0)+offset, Vector3(1, 10, 70));//left
}

void TutorialGame::InitGameExamples() {
	AddPlayerToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));
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

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
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



void TutorialGame::CreateRope(const Vector3& startPos, const Vector3& endPos, float interval) {
	Vector3 cubeSize = Vector3(0.3, 0.3, 0.3);

	float invCubeMass = 20.0f;        
	float maxDistance = interval*0.8 ; 
	int numLinks = static_cast<int>(Vector::Length(endPos - startPos) / interval); // Dynamically calculate the number of links

	GameObject* startStick = AddFloorToWorld(startPos,Vector3(0.5,1,0.5));
	GameObject* endStick = AddFloorToWorld(endPos,Vector3(0.5,1,0.5));
	GameObject* start = AddCubeToWorld(startPos, cubeSize, 0.0f); // Fixed point start
	GameObject* end = AddCubeToWorld(endPos, cubeSize, 0.0f);     // Fixed point end

	startStick->GetRenderObject()->SetColour(Vector4(0.8,0.9,0.9,1));
	endStick->GetRenderObject()->SetColour(Vector4(0.8,0.9,0.9,1));
	
	GameObject* previous = start;

	for (int i = 1; i <= numLinks; ++i) {
		// Calculate the position of each intermediate node
		Vector3 position = startPos + Vector::Normalise(endPos - startPos) * (interval * i);
		GameObject* block = AddCubeToWorld(position, cubeSize, invCubeMass);
		block->GetRenderObject()->SetDefaultTexture(metalTex);
		// Add constraint connection to the previous node
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);

		previous = block;
	}

	// Constraint between the last node and the endpoint
	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

void TutorialGame::CreateRopeGroup()
{
	CreateRope(Vector3(0,0,-5),Vector3(15,0,-5),0.7f);
	CreateRope(Vector3(0,0,10),Vector3(15,0,10),0.7f);
	CreateRope(Vector3(-10,0,30),Vector3(-10,0,40),0.7f);
	CreateRope(Vector3(-10,0,30),Vector3(-5,0,20),0.8f);
	CreateRope(Vector3(-5,0,50),Vector3(-10,0,40),0.8f);
	
}


StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position)
{
	StateGameObject* apple = new StateGameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
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
	floors.push_back(AddFloorToWorld(Vector3(45, 0, 12), Vector3(6, 1, 1)));
	floors.push_back(AddFloorToWorld(Vector3(70, 0, 12), Vector3(6, 1, 1)));
	floors.push_back(AddFloorToWorld(Vector3(60, 0, 30), Vector3(8, 1, 3)));
	floors.push_back(AddFloorToWorld(Vector3(45, 0, 50), Vector3(8, 1, 3)));
	floors.push_back(AddFloorToWorld(Vector3(70, 0, 50), Vector3(3, 1, 3)));
	floors.push_back(AddFloorToWorld(Vector3(35, 0, 70), Vector3(9, 1, 3)));
	floors.push_back(AddFloorToWorld(Vector3(65, 0, 70), Vector3(8, 1, 3)));
	floors.push_back(AddFloorToWorld(Vector3(10, 0, 50), Vector3(4, 1, 4)));
	floors.push_back(AddFloorToWorld(Vector3(25, 0, 50), Vector3(2, 1, 4)));

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

Door* TutorialGame::CreateDoor(const Vector3& position)
{
	Door* door = new Door();

	AABBVolume* volume = new AABBVolume(Vector3(1,1,1));
	door->SetBoundingVolume((CollisionVolume*)volume);
	door->GetTransform()
		.SetScale(Vector3(3, 6, 1))
		.SetPosition(position);

	door->SetRenderObject(new RenderObject(&door->GetTransform(), cubeMesh, nullptr, basicShader));
	door->SetPhysicsObject(new PhysicsObject(&door->GetTransform(), door->GetBoundingVolume()));

	door->GetPhysicsObject()->SetInverseMass(0.0f);
	door->GetPhysicsObject()->InitSphereInertia();

	GameObject* doorObj= CreateDoorObject(Vector3(20,0,0),Vector3(6,9,1));
	door->doorObject=doorObj;
	door->Init();

	world->AddGameObject(door);

	return door;
}

GameObject* TutorialGame::CreateDoorObject(const Vector3& position, const Vector3& size)
{
	GameObject* doorObject = new GameObject();
	doorObject->tag = "Ground";
	doorObject->SetName("floor");
	//Vector3 floorSize = Vector3(70, 2, 70);
	Vector3 floorSize = size;
	AABBVolume* volume = new AABBVolume(floorSize);
	doorObject->SetBoundingVolume((CollisionVolume*)volume);
	doorObject->GetTransform()
		.SetScale(floorSize * 2.0f)
		.SetPosition(position);

	doorObject->SetRenderObject(new RenderObject(&doorObject->GetTransform(), cubeMesh, floorTex, basicShader));
	doorObject->GetRenderObject()->SetColour(Vector4(1,1,1,1));
	doorObject->SetPhysicsObject(new PhysicsObject(&doorObject->GetTransform(), doorObject->GetBoundingVolume()));

	doorObject->GetPhysicsObject()->SetInverseMass(0);
	doorObject->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(doorObject);
	
	return doorObject;
}







