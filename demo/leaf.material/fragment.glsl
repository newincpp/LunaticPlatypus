#version 430 core

layout(location = 0) uniform float uTime;
layout(location = 1) uniform mat4 uMeshTransform;
layout(location = 2) uniform mat4 uView;

layout(location = 0) in vec3 vInfVertexPos_;
layout(location = 1) in vec3 vInfVertexNormal_;
layout(location = 2) in vec2 vInfUvCoord_;

out vec3 gPosition;
out vec4 gNormalRough;
out vec4 gAlbedoMetallic;

uniform layout(binding=2, rgba8) restrict image3D cheapGI;

vec3 getCameraPos() {
    return inverse(uView)[3].xyz;
}

void main() {
    gPosition = vInfVertexPos_;
    gNormalRough.xyz = vInfVertexNormal_;
    gAlbedoMetallic.rgb = vec3(0.0, 0.37, 0.1);

    gNormalRough.w = 0.65;
    gAlbedoMetallic.w = 0.01;

    // Fake Global Illumination (inspired by unreal's LPV)
    vec3 giBufferCenter = getCameraPos() - vec3(imageSize(cheapGI)) / 2.0f;
    const float texelToWorldUnitscaling = 8;
    ivec3 coord = ivec3(distance(giBufferCenter, vInfVertexPos_) / texelToWorldUnitscaling);
    vec4 currentColour = imageLoad(cheapGI, coord);
    if(currentColour.a > 0.5f) { // using alpha as a bool for "first set"
        imageStore(cheapGI, coord, vec4(currentColour.rgb * gAlbedoMetallic.rgb, 1.0));
    } else {
        imageStore(cheapGI, coord, vec4(gAlbedoMetallic.rgb, 1.0));
    }
}
