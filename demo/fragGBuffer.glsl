#version 430 core

layout(location = 0) uniform float uTime;
//uniform mat4 uCamera;
layout(location = 1) uniform mat4 uMeshTransform;

layout(location = 0) in vec3 vInfVertexPos_;
layout(location = 1) in vec3 vInfVertexNormal_;
layout(location = 2) in vec2 vInfUvCoord_;
layout(location = 3) in vec2 vInfDepth_;

out vec3 gPosition;
out vec3 gNormal;
out vec3 gAlbedoSpec;
//out float gDepth;

void main() {
    //if (gl_FragCoord.x > 500) {
    //    discard;
    //}
    // Store the fragment position vector in the first gbuffer texture
    gPosition = vInfVertexPos_;
    // Also store the per-fragment normals into the gbuffer
    gNormal = vInfVertexNormal_;
    // And the diffuse per-fragment color
    gAlbedoSpec.rgb = vec3(vInfUvCoord_, 0.0f);
    //gDepth = 1.0f;
}
