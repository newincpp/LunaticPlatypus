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
uniform layout(binding=1, rgba16f) writeonly image2D uFractalTexture; // float
//uniform layout(binding=1, rgba16ui) writeonly uimage2D uFractalTexture; // int

void main() {
    //if (gl_FragCoord.x > 500) {
    //    discard;
    //}
    // Store the fragment position vector in the first gbuffer texture
    gPosition = vInfVertexPos_;
    // Also store the per-fragment normals into the gbuffer
    gNormal = vInfVertexNormal_;
    // And the diffuse per-fragment color
    gAlbedoSpec.rgb = vec3(0.3, 0.3, 0.3);
    //gDepth = 1.0f;
    ivec2 plop = ivec2((gl_FragCoord.xy / vec2(1920.0f, 1080.0f)) * vec2(imageSize(uFractalTexture)));

    imageStore(uFractalTexture, plop, vec4(gNormal, 1.0));
}