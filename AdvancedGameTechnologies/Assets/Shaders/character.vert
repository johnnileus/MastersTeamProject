#version 400 core

uniform mat4 modelMatrix 	= mat4(1.0f);
uniform mat4 viewMatrix 	= mat4(1.0f);
uniform mat4 projMatrix 	= mat4(1.0f);
uniform mat4 shadowMatrix 	= mat4(1.0f);

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec4 jointWeights;
layout(location = 6) in ivec4 jointIndices;

uniform vec4 objectColour = vec4(1,1,1,1);

uniform bool hasVertexColours = false;
uniform mat4 joints[128];

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
	vec4 localPos = vec4(position, 1.0f);
	vec4 skelPos = vec4(0,0,0,0);

	for(int i = 0; i < 4; ++i){
		int jointIndex = jointIndices[i];
		float jointWeight = jointWeights[i];

		skelPos += joints[jointIndex] * localPos * jointWeight;
	}

	vec4 worldPos = (modelMatrix * vec4(skelPos.xyz, 1.0));
	OUT.worldPos = worldPos.xyz;
	gl_Position = (projMatrix * viewMatrix) * worldPos;

	mat3 normalMatrix = transpose ( inverse ( mat3 ( modelMatrix )));

	OUT.texCoord	= texCoord;
	OUT.colour.xyz	= objectColour.xyz;
	OUT.colour.a = 1;

	OUT.normal = normalize(normalMatrix * normalize(normal));

	OUT.shadowProj 	=  shadowMatrix * vec4 ( worldPos.xyz,1);

	if(hasVertexColours) {
		OUT.colour	= objectColour * colour;
	}
}