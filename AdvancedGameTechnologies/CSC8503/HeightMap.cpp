#include "HeightMap.h"
#include "stb_perlin.h"
#include <iostream>

HeightMap::HeightMap(const std::string& name) {
	int iWidth, iHeight, iChans;
	float data = stb_perlin_noise3(iWidth, iHeight, iChans, 0, 0, 0);
}