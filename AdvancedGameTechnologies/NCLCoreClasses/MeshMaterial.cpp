#include "MeshMaterial.h"
#include "Assets.h"
#include "TextureLoader.h"

using namespace NCL;
using namespace NCL::Rendering;
using std::ifstream;

MeshMaterial::MeshMaterial(const std::string& filename) {
	std::cout<<">>>>>>>>>>>>start load material<<<<<<<<<<<<<<"<<std::endl;
	ifstream file(Assets::MESHDIR + filename);

	string dataType;
	file >> dataType;

	if (dataType != "MeshMat") {
		std::cout << __FUNCTION__ << " File " << filename << " is not a MeshMaterial!\n";
		return;
	}
	int version;
	file >> version;

	if (version != 1) {
		std::cout << __FUNCTION__ << " File " << filename << " has incompatible version " << version << "!\n";
		return;
	}

	int matCount;
	int meshCount;
	file >> matCount;
	file >> meshCount;

	materialLayers.resize(matCount);

	for (int i = 0; i < matCount; ++i) {
		string name;
		int count;
		file >> name;
		file >> count;

		for (int j = 0; j < count; ++j) {
			string entryData;
			file >> entryData;
			string channel;
			string file;
			size_t split = entryData.find_first_of(':');
			channel = entryData.substr(0, split);
			file = entryData.substr(split + 1);

			materialLayers[i].entries.insert(std::make_pair(channel, std::make_pair(file, nullptr)));
		}
	}

	for (int i = 0; i < meshCount; ++i) {
		int entry;
		file >> entry;
		meshLayers.emplace_back(&materialLayers[entry]);
	}
}

const MeshMaterialEntry* MeshMaterial::GetMaterialForLayer(int i) const {
	if (i < 0 || i >= meshLayers.size()) {
		return nullptr;
	}
	return meshLayers[i];
}

void MeshMaterial::LoadTextures() {
	for(auto& i : meshLayers) {
		i->LoadTextures();
	}
}

void MeshMaterialEntry::LoadTextures() {
	for (auto& i : entries) {
		string filename = Assets::TEXTUREDIR + i.second.first;
		if (filename.find(".TGA") != string::npos || filename.find(".tga") != string::npos) {
			std::cout << "Ignoring TGA file: " << filename << std::endl;
			continue; // Skip loading this file, Theres some weird shenanigans to do with the PS5 and TGA files.
		}

		//Texture* t = TextureLoader::LoadAPITexture(filename);

		//i.second.second = t;
	}
}