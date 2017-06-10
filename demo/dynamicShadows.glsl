#version 430 core

layout(location = 0) uniform float uTime;
layout(location = 2) uniform mat4 uView;
layout(location = 3) uniform mat4 uProjection;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDepth;

layout(location = 0) in vec3 inVertexPos_;
out vec3 outColour;

layout(binding=1, rgba16f) uniform image2D uRaytracedShadowBuffer; // float
//uniform layout(binding=1, rgba16ui) uimage2D uFractalTexture; // int
vec2 resolution = vec2(1920, 1080);
#define PI    3.14159265

float timeBounce(float slow) {
    return sin(uTime / slow) / 2 + 0.5;
}

vec3 getCameraPos() {
    return vec3(17.500000, 4.000000, -7.700000);
    //return inverse(uView)[3].xyz;
}

vec3 getCameraRight() {
    return vec3(0.384261, -0.395495, 0.834222);
    //return inverse(uView)[0].xyz;
}
vec3 getCameraUp() {
    return vec3(0.000000, -0.903597, -0.428385);
    //return inverse(uView)[1].xyz;
}
vec3 getCameraForward() {
    return vec3(0.923225, 0.164611, -0.347217);
    //return inverse(uView)[2].xyz;
}

vec3 planeInt(vec3 normal, vec3 center, vec3 rayorg, vec3 raydir) {
    return (center - rayorg) + (raydir * (dot(center - rayorg, normal) / dot(normal, raydir)));
}

vec2 raytraceToCamera(vec3 raydir, vec3 rayorg) {
    vec3 cp = getCameraPos();
    vec3 cf = getCameraForward();
    vec3 cu = getCameraUp();
    vec3 cr = getCameraRight();

    if (dot(raydir, getCameraForward()) < 0.0f) {
        return vec2(-1.f, -1.f);
    }

    vec3 i = planeInt(cf, cp + cf * -0.43, rayorg, raydir);
    //i = (transpose(inverse(uProjection)) * vec4(i, 0.0f)).xyz;
    float sideT = dot(i, cu) - (PI / 2.);
    float sideBiT = dot(i, cr) - (PI / 2.);

    vec2 uv = -(vec2(sideBiT, sideT));
    uv -= vec2(1.0707, 1.2895);
    uv.y *= (resolution.x / resolution.y);
    return clamp(uv, 0.0, 1.0);
}

vec2 getUVfromPosition(vec3 position) {
    return raytraceToCamera(normalize(getCameraPos() - position), getCameraPos());
}

ivec2 UVCoordToTexelCoord(vec2 uv, image2D img) {
    return ivec2(uv * imageSize(img));
}

void main() {
    outColour = inVertexPos_;
    vec2 rtUV = getUVfromPosition(inVertexPos_);
    ivec2 projectedCoord = UVCoordToTexelCoord(rtUV, uRaytracedShadowBuffer);

    if (rtUV.x > 0.0f && rtUV.x < 1.0 && rtUV.y > 0.0 && rtUV.y < 1.0) {
	//imageStore(uRaytracedShadowBuffer, ivec2(gl_FragCoord.xy), vec4(rtUV, 0.0f, 1.0f));
	imageStore(uRaytracedShadowBuffer, ivec2(gl_FragCoord.xy), texture2D(gAlbedoSpec, rtUV));

	//vec3 nearestGBufferPosition = texture2D(gPosition, rtUV).xyz;
	//float d = distance(inVertexPos_, nearestGBufferPosition);
	//if (d) {
	//}
	//imageStore(uRaytracedShadowBuffer, ivec2(gl_FragCoord.xy), texture2D(gAlbedoSpec, rtUV));
    } else {
	imageStore(uRaytracedShadowBuffer, ivec2(gl_FragCoord.xy), vec4(0.0, 0.0, 0.0f, 1.0f));
    }
    return;

    discard; // this shader is used as a compute shader with a raster output in parmater;
}
