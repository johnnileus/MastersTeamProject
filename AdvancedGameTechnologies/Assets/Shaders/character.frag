#version 420 core
#extension GL_ARB_bindless_texture : enable

layout(bindless_sampler) uniform sampler2D bindless;
uniform textures{
	sampler2D arr[30];
} bindless_textures;

layout (std140) uniform materials{
	vec4 key[100];
} materialReferencer;


struct ImpactPoint{
	vec3 position;
	int colour;
	float radius;
};

struct Light{
	vec4 colour;
	vec3 position;
	float radius;
};

layout (std140) uniform lights{
	uniform Light sceneLight;
};

#define TEAM_COUNT 4
layout (std140) uniform teamColours{
	vec4 teamColour[TEAM_COUNT];
};

#define MAX_IMPACT_POINTS 300
uniform ImpactPoint impactPoints[MAX_IMPACT_POINTS];

uniform int impactPointCount;
uniform int materialIndex = -1;

uniform vec4 		objectColour;

layout(binding = 7) uniform sampler2D 	mapTex;

uniform sampler2DShadow shadowTex;

uniform mat4 modelMatrix 	= mat4(1.0f);

uniform vec3	cameraPos;

uniform vec3 objectPosition;

uniform bool hasTexture;
uniform bool isDynamic;
uniform bool isPlayer;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 worldPosition;
	vec3 position;
} IN;

out vec4 fragColor;

vec4 modulus(vec4 x){return x - floor(x * (1.0/289.0)) * 289.0;}
vec4 permutate(vec4 x){return modulus(((x * 34.0) + 1.0) * x);}

float SplatNoise(vec3 inWorldPos){
	vec3 floorP = floor(inWorldPos);
	vec3 dist = inWorldPos - floorP;
	dist = dist * dist * (3.0 - 2.0 * dist);

	vec4 b = floorP.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
	vec4 permB = permutate(b.xyxy);
	vec4 permB2 = permutate(permB.xyxy + b.zzww);

	vec4 c = permB2 + floorP.zzzz;
	vec4 permC = permutate(c);
	vec4 permC2 = permutate(c + 1.0);

	vec4 fractalC = fract(permC * (1.0/41.0));
	vec4 fractalC2 = fract(permC2 * (1.0/41.0));

	vec4 shapePass = fractalC2 * dist.z + fractalC * (1.0 - dist.z);
	vec2 shapePass2 = shapePass.yw * dist.x + shapePass.xz * (1.0 - dist.x);

	return shapePass2.y * dist.y + shapePass2.x * (1.0 - dist.y);
}

void main(void)
{
	float shadow = 1.0;

	if(IN.shadowProj.w > 0.0) {
		shadow = textureProj(shadowTex , IN.shadowProj);
	}

	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

	vec3 bumpNormal;

	vec4 linkedMaterial = materialReferencer.key[materialIndex];
	if(linkedMaterial.y != -1) {
		bumpNormal = texture(bindless_textures.arr[int(linkedMaterial.y)], IN.texCoord).rgb;
		bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));
	}
	else{
		bumpNormal = IN.normal;
	}
	vec3  incident = normalize ( sceneLight.position - IN.worldPos );
	float lambert  = max (0.0 , dot ( incident , bumpNormal )) * 0.9;

	vec3 viewDir = normalize ( cameraPos - IN.worldPos );
	vec3 halfDir = normalize ( incident + viewDir );

	float rFactor = max (0.0 , dot ( halfDir , bumpNormal));
	float sFactor = pow ( rFactor , 80.0 );

	vec4 albedo = IN.colour;

	if (linkedMaterial.x != -1){
		albedo *= texture(bindless_textures.arr[int(linkedMaterial.x)], IN.texCoord);
	}

	for (int i = 0; i < impactPointCount; i++){

		vec4 localImpactPos = modelMatrix * vec4(impactPoints[i].position, 1.0);
		float distanceBetween = (isDynamic == true && isPlayer == false) ? distance(IN.worldPosition.xyz, localImpactPos.xyz) : distance(IN.worldPosition.xyz, impactPoints[i].position + objectPosition);
		float splat = (isDynamic == true && isPlayer == false) ? SplatNoise((IN.position)*(5+(0.1*(mod(i, 10))))) : SplatNoise((IN.worldPosition.xyz - objectPosition)*(5+(0.1*(mod(i, 10)))));
		if (distanceBetween <= impactPoints[i].radius - splat){
			albedo = teamColour[impactPoints[i].colour];
		}
	}

	albedo.rgb = pow(albedo.rgb, vec3(2.2));

	fragColor.rgb = albedo.rgb * 0.05f; //ambient

	fragColor.rgb += albedo.rgb * sceneLight.colour.rgb * lambert * shadow; //diffuse light 

	fragColor.rgb += sceneLight.colour.rgb * sFactor * shadow * 0.2f; //specular // Change once bump-maps are introduced 

	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2f));

	fragColor.a = albedo.a;
}