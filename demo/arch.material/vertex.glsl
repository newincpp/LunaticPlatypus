#version 430 core

layout(location = 0) uniform float uTime;
//layout(location = 1) uniform mat4 uCamera;
layout(location = 1) uniform mat4 uMeshTransform;
layout(location = 2) uniform mat4 uView;
layout(location = 3) uniform mat4 uProjection;

layout(location = 0) in vec3 vertexPos_;
layout(location = 1) in vec3 vertexNormal_;
layout(location = 2) in vec2 uvCoord_;

layout(location = 0) out vec3 vInfVertexPos_;
layout(location = 1) out vec3 vInfVertexNormal_;
layout(location = 2) out vec2 vInfUvCoord_;
layout(location = 3) out vec2 vInfDepth_;

float timeBounce(float slow) {
    return sin(uTime / slow) / 2 + 0.5;
}

vec3 random(vec3 coord, float noiseamount) {
    float noiseX = ((fract(1.0 - coord.x * (500 / 2.0)) * 0.25) + (fract(coord.y * (500 / 2.0)) * 0.75)) * 2.0 - 1.0 + fract(coord.z * (500 / 2.0)) * 0.75;
    float noiseY = ((fract(1.0 - coord.x * (500 / 2.0)) * 0.75) + (fract(coord.y * (500 / 2.0)) * 0.25)) * 2.0 - 1.0 + fract(coord.z * (500 / 2.0)) * 0.25;
    float noiseZ = ((fract(1.0 - coord.x * (500 / 2.0)) * 0.50) + (fract(coord.y * (500 / 2.0)) * 0.50)) * 2.0 - 1.0 + fract(coord.z * (500 / 2.0)) * 0.50;
    vec4 n = vec4(vec3(noiseX, noiseY, noiseZ) * 0.0001 * noiseamount, 1.0);
    return (uProjection * n).xyz;
}

void main() {    
    //vec3 displacement = vec3(sin(uTime * .001), 0.0f, sin(uTime*0.003));
    //vec3 displacement = (vertexNormal_ / 10) * sin(distance(vec3(0.0), vertexPos_ + (uTime / 600)));
    vec3 displacement = vec3(0.);
    gl_Position = uProjection * uView * uMeshTransform * vec4(vertexPos_ + displacement, 1.0);

    vInfVertexPos_ = (uMeshTransform * vec4(vertexPos_ + displacement, 1.0)).xyz;
    //vInfVertexNormal_ = vertexNormal_;
    vInfVertexNormal_ = (transpose(inverse(uView * uMeshTransform )) * vec4(vertexNormal_, 1.0)).xyz / 2 + 0.5;
    vInfUvCoord_ = uvCoord_;
}  
