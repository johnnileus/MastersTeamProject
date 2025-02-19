#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"
#include "HeightMap.h"

HeightMap::HeightMap(int size, float scale, float heightMultiplier) : size(size), scale(scale), heightMultiplier(heightMultiplier) {
	heightmap.resize(size, std::vector<float>(size, 0.0f));
	Generate();
}

HeightMap::~HeightMap() {}

void HeightMap::Generate() {
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			float nx = x * scale;
			float ny = y * scale;

			float noiseValue = stb_perlin_noise3(nx, ny, 0.0f, 0, 0, 0);
			heightmap[x][y] = (noiseValue + 1.0f) * 0.5f * heightMultiplier;
		}
	}
}

float HeightMap::GetHeight(int x, int y) const {
	if (x >= 0 && x < size && y >= 0 && y < size) {
		return heightmap[x][y];
	}
	return 0.0f;
}