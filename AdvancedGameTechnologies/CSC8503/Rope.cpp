#include "Rope.h"

#include "PositionConstraint.h"
#include "SceneManager.h"

Rope::Rope()
{
    
}

/// Create a rope using a given start and end point.
/// The number of rope segments is automatically determined based on the specified segment interval.
/// @param world current world
/// @param startPos where the rope start
/// @param endPos where the rope end
/// @param interval each rope node's interval
/// @return The GameObject of the fixed starting point
GameObject* Rope::AddRopeToWorld(GameWorld* world, const Vector3& startPos, const Vector3& endPos, float interval)
{
    Vector3 segmentSize = Vector3(0.3, 0.3, 0.3);

    float invSegmentMass = 20.0f;        
    float maxDistance = interval*0.8;
    int numLinks = static_cast<int>(Vector::Length(endPos - startPos) / interval); // Dynamically calculate the number of links

    GameObject* startStick = SceneManager::Instance().AddCubeToWorld(world, startPos,Vector3(0.5,1,0.5),0);
    GameObject* endStick = SceneManager::Instance().AddCubeToWorld(world, endPos,Vector3(0.5,1,0.5),0);

    GameObject* start = SceneManager::Instance().AddCubeToWorld (world, startPos, segmentSize, 0.0f); // Fixed point start
    GameObject* end = SceneManager::Instance().AddCubeToWorld(world, endPos, segmentSize, 0.0f);     // Fixed point end

    startStick->GetRenderObject()->SetColour(Vector4(0.8,0.9,0.9,1));
    endStick->GetRenderObject()->SetColour(Vector4(0.8,0.9,0.9,1));
	
    GameObject* previous = start;

    for (int i = 1; i <= numLinks; ++i) {
        // Calculate the position of each intermediate node
        Vector3 position = startPos + Vector::Normalise(endPos - startPos) * (interval * i);
        GameObject* block = SceneManager::Instance().AddCubeToWorld(world, position, segmentSize, invSegmentMass);
        block->GetRenderObject()->SetDefaultTexture(AssetManager::Instance().metalTex);
        // Add constraint connection to the previous node
        PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
        world->AddConstraint(constraint);

        previous = block;
    }

    // Constraint between the last node and the endpoint
    PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
    world->AddConstraint(constraint);

    return startStick;
}

