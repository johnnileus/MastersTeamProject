#version 330 core

in vec2 uv;               
out vec4 fragColor;        

uniform sampler2D sceneTex; 


uniform float tintStrength = 0.3; 

void main() {
    vec4 sceneColor = texture(sceneTex, uv);

    vec3 yellowColor = vec3(1.0, 1.0, 0.0); 
    vec3 mixColor    = mix(sceneColor.rgb, yellowColor * sceneColor.rgb, tintStrength);

    fragColor = vec4(mixColor, sceneColor.a);
}
