#version 430 core

layout(location = 0) uniform float uTime;
layout(location = 1) uniform mat4 uMeshTransform;
layout(location = 2) uniform mat4 uView;
layout(location = 3) uniform mat4 uProjection;

layout(location = 0) in vec3 vertexPos_;
layout(location = 1) in vec3 vertexNormal_;
layout(location = 2) in vec2 uvCoord_;

layout(location = 0) out vec3 outVertexPos_;

float timeBounce(float slow) {
    return sin(uTime / slow) / 2 + 0.5;
}

void main() {    
    //vec3 displacement = vec3(sin(uTime * .001), 0.0f, sin(uTime*0.003));
    //vec3 displacement = (vertexNormal_ / 10) * sin(distance(vec3(0.0), vertexPos_ + (uTime / 600)));
    vec3 displacement = vec3(0.);
    gl_Position = uProjection * uView * uMeshTransform * vec4(vertexPos_ + displacement, 1.0);

    outVertexPos_ = (uMeshTransform * vec4(vertexPos_ + displacement, 1.0)).xyz;
}
