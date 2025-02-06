#include "HeightMap.h"
#include "stb_perlin.h"
#include <iostream>

using namespace NCL::Rendering;

HeightMap::HeightMap(const std::string& name) {
	int iWidth = 256;
	int iHeight = 256;
	GLuint numVertices = iWidth * iHeight;
	GLuint numIndices = (iWidth - 1) * (iHeight - 1) * 6;
	Vector3* vertices = new Vector3[numVertices];
	Vector2* textureCoords = new Vector2[numVertices];
	GLuint* indices = new GLuint[numIndices];

	Vector3 vertexScale = Vector3(16.0f, 1.0f, 16.0f);
	Vector2 textureScale = Vector2(1 / 16.0f, 1 / 16.0f);

	float* perlinNoise = new float[iWidth * iHeight];

	float scaleX = 0.1f;
	float scaleY = 0.1f;

	for (int z = 0; z < iHeight; ++z) {
		for (int x = 0; x < iWidth; ++x) {
			float noiseValue = stb_perlin_noise3((float)x * scaleX, (float)z * scaleY, 0.0f, 0.5, 0.5, 4);
			perlinNoise[z * iWidth + x] = noiseValue;

			int offset = (z * iWidth) + x;
			vertices[offset] = Vector3(x, noiseValue * 10.0f, z) * vertexScale;
			textureCoords[offset] = Vector2(x, z) * textureScale;
		}
	}

	delete[] perlinNoise;

	int i = 0;

	for (int z = 0; z < iHeight; ++z) {
		for (int x = 0; x < iWidth - 1; ++x) {
			int a = (z * (iWidth)) + x;
			int b = (z * (iWidth)) + (x + 1);
			int c = ((z + 1) * (iWidth)) + (x + 1);
			int d = ((z + 1) * (iWidth)) + x;

			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;

			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}
	OGLMesh::UploadToGPU;

	heightmapSize.x = vertexScale.x * (iWidth - 1);
	heightmapSize.y = vertexScale.y * 255.0f;
	heightmapSize.z = vertexScale.z * (iHeight - 1);
}
