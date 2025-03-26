#version 400 core

uniform mat4 modelMatrix 	= mat4(1.0f);
uniform mat4 viewMatrix 	= mat4(1.0f);
uniform mat4 projMatrix 	= mat4(1.0f);
uniform mat4 shadowMatrix 	= mat4(1.0f);

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in ivec4 jointIndices;
layout(location = 5) in vec4 jointWeights;

uniform vec4 objectColour = vec4(1,1,1,1);

uniform mat4 joints[128];//joint matrices

uniform bool hasVertexColours = false;

out Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} OUT;

void main(void)
{
	//calculate skin: weighted vertex positions and normals.
	vec4 skinnedPos = vec4(0.0);
	vec4 skinnedNormal = vec4(0.0);
	for (int i = 0; i < 4; i++) {
		int idx = jointIndices[i];
		float weight = jointWeights[i];
		skinnedPos += joints[idx] * vec4(position, 1.0) * weight;
		skinnedNormal += joints[idx] * vec4(normal, 0.0) * weight;
	}
	//convert to World Space
	vec4 finalPos = modelMatrix * skinnedPos;

	//calculate normal matrix and convert skinned normal
	mat3 normalMatrix = transpose ( inverse ( mat3 ( modelMatrix )));
	vec3 finalNormal = normalize(normalMatrix * skinnedNormal.xyz);
	
	//calculate the MVP matrix
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * skinnedPos;

	OUT.shadowProj 	= shadowMatrix * vec4 ( position,1) * finalPos;
	OUT.worldPos 	= finalPos.xyz;
	OUT.normal 		= finalNormal;
	OUT.texCoord	= texCoord;
	OUT.colour		= objectColour;

//	vec4 baseColour = objectColour;
//	if(hasVertexColours) {
//		baseColour = objectColour * colour;
//	}
//	OUT.colour = baseColour;
	//gl_Position		= mvp * vec4(position, 1.0);
}