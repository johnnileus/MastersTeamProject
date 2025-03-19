/******************************************************************************
This file is part of the Newcastle PS5 Tutorial Series

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#include "AGCMesh.h"
#include "AGCRenderer.h"
#include "PS5MemoryAllocator.h"

using namespace NCL;
using namespace PS5;

////These are both carefully arranged to match the MeshBuffer enum class!
sce::Agc::Core::TypedFormat typedFormats[] = {
	sce::Agc::Core::TypedFormat::k32_32_32Float,	//Positions have this format
	sce::Agc::Core::TypedFormat::k32_32_32_32Float,	//Colours
	sce::Agc::Core::TypedFormat::k32_32Float,		//TexCoords
	sce::Agc::Core::TypedFormat::k32_32_32Float,	//Normals
	sce::Agc::Core::TypedFormat::k32_32_32_32Float,	//Tangents are 4D!
	sce::Agc::Core::TypedFormat::k32_32_32_32Float,	//Skel Weights
	sce::Agc::Core::TypedFormat::k32_32_32_32UInt,	//Skel indices
};

//Attribute sizes for each of the above
size_t attributeSizes[] = {
	sizeof(Vector3),
	sizeof(Vector4),
	sizeof(Vector2),
	sizeof(Vector3),
	sizeof(Vector4),
	sizeof(Vector4),
	sizeof(Vector4),
};

AGCMesh::AGCMesh() {
	std::cout << "AGCMesh created: " << this << std::endl;
	vertexData	= nullptr;
	indexData	= nullptr;
}

void AGCMesh::UploadToGPU(Rendering::RendererBase* renderer) {
	std::cout << "AGCMesh::UploadToGPU" << std::endl;

	const MemoryAllocator& allocator = ((AGCRenderer*)renderer)->GetAllocator();

	int vertexCount = GetVertexCount();
	std::cout << "Vertex Count UploadToGPU: " << vertexCount << std::endl;

	int vertexSize = 0;

	auto attributeCountFunc = [&](VertexAttribute::Type v, size_t count) {
		if (count > 0) {
			vertexSize += attributeSizes[(uint32_t)v];
		}
	};

	attributeCountFunc(VertexAttribute::Positions		, GetPositionData().size()		);
	attributeCountFunc(VertexAttribute::Colours			, GetColourData().size()		);
	attributeCountFunc(VertexAttribute::TextureCoords	, GetTextureCoordData().size()	);
	attributeCountFunc(VertexAttribute::Normals			, GetNormalData().size()		);
	attributeCountFunc(VertexAttribute::Tangents		, GetTangentData().size()		);
	attributeCountFunc(VertexAttribute::JointWeights	, GetSkinWeightData().size()	);
	attributeCountFunc(VertexAttribute::JointIndices	, GetSkinIndexData().size()		);

	vertexData = (char*)allocator.Allocate((uint64_t)(vertexSize * vertexCount), sce::Agc::Alignment::kBuffer);
	std::cout << "Checking if Index Count > 0 in AGCMesh::UploadToGPU" << std::endl;
	if (GetIndexCount() > 0) {
		std::cout << "Index Count > 0 in AGCMesh::UploadToGPU: " << GetIndexCount() << std::endl;
		indexData = (char*)allocator.Allocate((uint64_t) sizeof(int) * GetIndexCount(), sce::Agc::Alignment::kBuffer);

		memcpy(indexData, GetIndexData().data(), sizeof(int) * GetIndexCount());
	}

	size_t bufferOffset = 0;

	auto attributeBuildFunc = [&, this](VertexAttribute::Type v, size_t count, const char* inputData, size_t& outputOffset) {
		uint32_t index = (uint32_t)v;

		if (count > 0) {
			sce::Agc::Core::Buffer vBuffer;

			size_t dataSize = attributeSizes[index] * count; 

			memcpy(vertexData + bufferOffset, inputData, dataSize);

			sce::Agc::Core::BufferSpec bufSpec;
			bufSpec.initAsRegularBuffer(vertexData + bufferOffset, attributeSizes[index], vertexCount);

			SceError error = sce::Agc::Core::initialize(&vBuffer, &bufSpec);

			usedVertexBuffers.push_back(vBuffer);
			usedAttributeTypes.push_back(v);
			std::cout << "usedAttributeTypes size: " << usedAttributeTypes.size() << std::endl;
			std::cout << "usedVertexBuffers size: " << usedVertexBuffers.size() << std::endl;

			bufferOffset += dataSize;
		}
	};

	attributeBuildFunc(VertexAttribute::Positions	, GetPositionData().size()		, (const char*)GetPositionData().data()		, bufferOffset);
	std::cout << "GetPositionData().size(): " << GetPositionData().data() << std::endl;
	attributeBuildFunc(VertexAttribute::Colours		, GetColourData().size()		, (const char*)GetColourData().data()		, bufferOffset);
	std::cout << "GetColourData().size(): " << GetColourData().data() << std::endl;
	attributeBuildFunc(VertexAttribute::TextureCoords, GetTextureCoordData().size()	, (const char*)GetTextureCoordData().data()	, bufferOffset);
	std::cout << "GetTextureCoordData().size(): " << GetTextureCoordData().data() << std::endl;
	attributeBuildFunc(VertexAttribute::Normals		, GetNormalData().size()		, (const char*)GetNormalData().data()		, bufferOffset);
	std::cout << "GetNormalData().size(): " << GetNormalData().data() << std::endl;
	attributeBuildFunc(VertexAttribute::Tangents	, GetTangentData().size()		, (const char*)GetTangentData().data()		, bufferOffset);
	std::cout << "GetTangentData().size(): " << GetTangentData().data() << std::endl;
	attributeBuildFunc(VertexAttribute::JointWeights, GetSkinWeightData().size()	, (const char*)GetSkinWeightData().data()	, bufferOffset);
	std::cout << "GetSkinWeightData().size(): " << GetSkinWeightData().data() << std::endl;
	attributeBuildFunc(VertexAttribute::JointIndices, GetSkinIndexData().size()		, (const char*)GetSkinIndexData().data()	, bufferOffset);
	std::cout << "GetSkinIndexData().size(): " << GetSkinIndexData().data() << std::endl;
}

void AGCMesh::BindVertexBuffers(sce::Agc::Core::StageBinder& binder) {
	std::cout << "AGCMesh::BindVertexBuffers" << std::endl;
	if (usedVertexBuffers.empty()  /*|| usedVertexAttributes.empty()*/) {
		std::cout << "No vertex buffers to bind!" << std::endl;
		return;
	}

	const int TEST_OFFSET = 10;

	for (int i = 0; i < usedVertexBuffers.size(); ++i) {
		binder.setBuffers((int)usedAttributeTypes[i] + TEST_OFFSET, 1, &usedVertexBuffers[i]);
	}
	std::cout << "usedAttributeTypes in BindVertex size: " << usedAttributeTypes.size() << std::endl;
	std::cout << "usedVertexBuffers in BindVertex size: " << usedVertexBuffers.size() << std::endl;
}

char* AGCMesh::GetAGCIndexData() const {
	std::cout << "AGCMesh::GetAGCIndexData" << std::endl;
	return indexData;
}

bool	AGCMesh::GetAGCBuffer(NCL::Rendering::VertexAttribute::Type type, sce::Agc::Core::Buffer& outBuffer) {
	std::cout << "AGCMesh::GetAGCBuffer" << type << std::endl;
	std::cout << "usedAttributeTypes in GetBuffer size: " << usedAttributeTypes.size() << std::endl;
	std::cout << "usedVertexBuffers in GetBuffer size: " << usedVertexBuffers.size() << std::endl;
	std::cout << "Searching for type: " << type << std::endl;
	for (int i = 0; i < usedAttributeTypes.size(); ++i) {
		if (usedAttributeTypes[i] == type) {
			outBuffer = usedVertexBuffers[i]; 
			return true;
		}
	}
	return false;
}