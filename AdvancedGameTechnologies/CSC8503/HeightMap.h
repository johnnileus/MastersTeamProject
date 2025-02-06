#pragma once
#include <string>
#include "OGLMesh.h"
using namespace NCL::Maths;

class HeightMap {
public:
	HeightMap(const std::string& name);
	~HeightMap(void) {};

	Vector3 GetHeightMapSize() const { return heightmapSize; }

protected:
	Vector3 heightmapSize;
};

/*
heightmap.cpp - logic for terrain gen
generate perlin noise

assetmanager.cpp - load textures for terrain
scenemanager.cpp - load heightmap and initialise terrain gen (renderer.cpp equiv)
tutorialgame.cpp - initialise/delete above
*/