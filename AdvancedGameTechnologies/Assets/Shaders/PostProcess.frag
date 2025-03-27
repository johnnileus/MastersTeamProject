#version 330 core

in  vec2 uv;
out vec4 fragColor;


uniform sampler2D sceneTex;
uniform sampler2D depthTex;

uniform float cameraNear;
uniform float cameraFar;

uniform vec3 fogColor = vec3(0.5, 0.5, 0.5);

uniform float fogDensity = 0.02;

float LinearizeDepth(in float ndcDepth) {

    float z = ndcDepth * 2.0 - 1.0; 
    return (2.0 * cameraNear * cameraFar) / (cameraFar + cameraNear - z * (cameraFar - cameraNear));
}

void main() {

    vec4 sceneColor = texture(sceneTex, uv);
    
    float rawDepth   = texture(depthTex, uv).r;
    float linearDist = LinearizeDepth(rawDepth);
    
    float fogFactor = 1.0 - exp(-fogDensity * linearDist);

    vec3 finalColor = mix(sceneColor.rgb, fogColor, fogFactor);

    fragColor = sceneColor;
}

