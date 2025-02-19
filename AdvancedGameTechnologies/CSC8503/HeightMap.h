#pragma once
#include <vector>
#include "Vector.h"

class HeightMap {
public:
	HeightMap(int size, float scale, float heightMult);
	~HeightMap();

	void Generate();
	float GetHeight(int x, int y) const;
	const std::vector < std::vector<float>>& GetData() const { return heightmap; }

	int GetSize() const { return size; }

protected:
	int size;
	float scale;
	float heightMultiplier;
	std::vector<std::vector<float>> heightmap;
};

