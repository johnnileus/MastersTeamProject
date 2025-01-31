#pragma once
#include <string>
#include "Mesh.h"
#include "Vector.h"
using namespace NCL::Maths;

class HeightMap {
public:
	HeightMap(const std::string& name);
	~HeightMap(void) {};

	Vector3 GetHeightmapSize() const {
		return heightmapSize;
	}

protected:
	Vector3 heightmapSize;
};
