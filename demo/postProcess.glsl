#version 430 core

layout(location = 0) uniform float uTime;
layout(location = 1) uniform mat4 uMeshTransform;
layout(location = 2) uniform mat4 uView;
layout(location = 3) uniform mat4 uProjection;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDepth;

layout(binding=1, rgba16f) uniform image2D uFractalTexture; // float
//uniform layout(binding=1, rgba16ui) uimage2D uFractalTexture; // int

in vec2 TexCoords;

out vec3 outColour;

vec2 resolution = vec2(1920, 1080);
float zNear = 0.1;
float zFar = 100.0;
vec3 CurrentNormal;
vec3 CurrentNormalWorldSpace;
vec3 CurrentNormalWorld;
vec3 CurrentPosition;
float CurrentDepth;
float CurrentDepthNotLinearized;


#define PI    3.14159265

float linearizeDepth(float z) {
    //float near = 0.1;  // the near plane
    //float far = 100.0; // the far plane
    return (2.0 * zNear) / (zFar + zNear - z * (zFar - zNear));
}

vec3 getCameraRight() {
    return inverse(uView)[0].xyz;
}
vec3 getCameraUp() {
    return inverse(uView)[1].xyz;
}
vec3 getCameraForward() {
    return inverse(uView)[2].xyz;
}
vec3 getCameraPos() {
    return inverse(uView)[3].xyz;
}

float timeBounce(float slow) {
    return sin(uTime / slow) / 2 + 0.5;
}

float fast_fresnel(float IOR) {
    float R0 = ((-1/(IOR+2.6)) * 3.2) + 0.75;
    return  R0 + (1.0f - R0) * exp(((1.0f - dot(-normalize(CurrentPosition - getCameraPos()), CurrentNormalWorldSpace)) - 1) * 4.6);
}

float AccurateFresnel(float IOR) {
    const float R0 = ((1.0f - IOR) * (1.0f - IOR)) / ((1.0f + IOR) * (1.0f + IOR));
    vec3 normal = (inverse(uView) * vec4(CurrentNormal,0.0f)).xyz;
    return R0 + (1.0 - R0) * pow( (1.0 - dot(-normalize(CurrentPosition - getCameraPos()), normal ) ), 5.0);
}

float fresnel(float IOR) {
    return AccurateFresnel(IOR);
    float t = timeBounce(500);
    if (t < 0.5) {
	return fast_fresnel(IOR);
    } else {
	return AccurateFresnel(IOR);
    }
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec2 randomFromUv(vec2 coord) { //generating noise/pattern texture for dithering
    float noiseamount = 0.0002; //dithering amount
    float noiseX = ((fract(1.0 - coord.s * (resolution.x / 2.0)) * 0.25) + (fract(coord.t * (resolution.y / 2.0)) * 0.75)) * 2.0 - 1.0;
    float noiseY = ((fract(1.0 - coord.s * (resolution.x / 2.0)) * 0.75) + (fract(coord.t * (resolution.y / 2.0)) * 0.25)) * 2.0 - 1.0;
    return vec2(noiseX, noiseY) * noiseamount;
}

vec3 planeInt(vec3 normal, vec3 center, vec3 rayorg, vec3 raydir) {
    return (center - rayorg) + (raydir * (dot(center - rayorg, normal) / dot(normal, raydir)));
}

vec2 raytraceToCamera(vec3 raydir, vec3 rayorg) {
    vec3 cp = getCameraPos();
    vec3 cf = getCameraForward();
    vec3 cu = getCameraUp();
    vec3 cr = getCameraRight();

    vec3 i = planeInt(cf, cp + cf * 0.218, rayorg, raydir);
    //i = (transpose(inverse(uProjection)) * vec4(i, 0.0f)).xyz;
    float sideT = dot(i, cu) - (PI / 2.);
    float sideBiT = dot(i, cr) - (PI / 2.);

    vec2 uv = -(vec2(sideBiT, sideT));
    uv -= vec2(1.0707, 1.2895);
    uv.y *= (resolution.x / resolution.y);
    return uv;
}

vec2 getUVfromPosition(vec3 position) {
    return raytraceToCamera(normalize(getCameraPos() - position), getCameraPos());
}

bool isRayTowardCamera(vec3 reflec) {
    vec2 uv = raytraceToCamera(reflec, CurrentPosition);
    return (uv.x >= 0 && uv.x <= 1) && (uv.y >= 0 && uv.y <= 1);
}


float doMist(float miststart, float mistend) {
	float zdepth = texture2D(gDepth, TexCoords.xy).x;
	float depth = -zFar * zNear / (zdepth * (zFar - zNear) - zFar);
	return clamp((depth - miststart) / mistend, 0.0, 1.0);
}

float readDepth(vec2 coord) {
	if (TexCoords.x < 0.0 || TexCoords.y < 0.0) {
	    return 1.0;
	}
	float z_b = texture2D(gDepth, coord).x;
	float z_n = 2.0 * z_b - 1.0;
	return (2.0 * zNear) / (zFar + zNear - z_n * (zFar - zNear));
}

int compareDepthsFar(float depth1, float depth2, float gaussDisplace) {
	//float garea = 2.0; //gauss bell resolution.x
	float diff = (depth1 - depth2) * 100.0; //depth difference (0-100)
	//reduce left bell resolution.x to avoid self-shadowing
	return diff < gaussDisplace ? 0 : 1;
	//if (diff<gaussDisplace) {
	//	return 0;
	//} else {
	//	return 1;
	//}
}

float compareDepths(float depth1, float depth2, float gaussDisplace) {
	float garea = 2.0; //gauss bell resolution.x
	float diff = (depth1 - depth2) * 100.0; //depth difference (0-100)
	//reduce left bell resolution.x to avoid self-shadowing

	float diffarea = 0.3; //self-shadowing reduction
	garea = diff < gaussDisplace ? diffarea : garea;
	//if (diff < gdisplace) {
	//	garea = diffarea;
	//}

	return pow(2.7182, -2.0 * pow((diff - gaussDisplace) / garea, 2.0));
}

float directionMultiplier(float coord, float texCoord, float normal, float cn) {
    return coord - texCoord < 0 ? (normal - cn > 0 ? 0.0 : 1.0) : normal - cn > 0 ? 1.0 : 0.0;
}

float dotDirection(vec3 a, vec3 b) {
    return sign(a.x * -b.y + a.y * b.x);
}

float normalStuff(vec3 normal, vec2 coord) {
    //float mul = //directionMultiplier(coord.x, TexCoords.x, normal.x, cn.x);
    //directionMultiplier(coord.x, TexCoords.x, normal.z, cn.z) *
    //directionMultiplier(coord.x, TexCoords.x, normal.y, cn.y);
    vec3 cn = texture2D(gNormal, coord).xyz;
    float mul = 1.0;
    vec3 c = cross(cn, normal);
    float angle = (atan(length(c), dot(normal, cn)));
    angle = (dot(cn, normal));
    mul = angle > 0 ? angle < PI/2.0 ? 1.0 : 0.0 : 0.0;
    //angle = degrees(angle);
    angle = 0.0;
    //angle = radians(angle);


    mul = dot(c, vec3(.0, .0, 1.0)) > 0 ? 1.0 : 0.0;
    c = cross(normal, cn);
    mul += dot(c, vec3(.0, .0, 1.0)) > 0 ? 1.0 : 0.0;
    //mul = dotDirection(normal, cn);
    return 1.0 - abs(dot(cn, normal));
    return (1.0 - angle) * mul;
}

float calAO(float depth, float dw, float dh, vec3 normal, float radius) {
	float dd = (1.0 - depth) * radius;

	float temp = 0.0;
	//float temp2 = 0.0;
	float coordw = TexCoords.x + dw*dd;
	float coordh = TexCoords.y + dh*dd;
	float coordw2 = TexCoords.x - dw*dd;
	float coordh2 = TexCoords.y - dh*dd;

	vec2 coord = vec2(coordw , coordh);
	vec2 coord2 = vec2(coordw2, coordh2);

	float cd = readDepth(coord); //11ms
	float gdisplace = 0.4; //gauss bell center //0.4
	temp = compareDepths(depth, cd, gdisplace) * normalStuff(normal, coord);
	//DEPTH EXTRAPOLATION: 5ms
	int far = compareDepthsFar(depth, cd, gdisplace);
	if (far > 0) {
	    temp += (1.0 - temp) * compareDepths(readDepth(coord2), depth, gdisplace) * normalStuff(normal, coord2);
	}

	return temp;
}

float ssao(float samples, float radius) {
    float aoclamp = 0.125; //depth clamp - reduces haloing at screen edges
    float strength = 1.2;

    vec2 noise = randomFromUv(TexCoords);
    float depth = readDepth(TexCoords);

    float w = (1.0 / resolution.x) / clamp(depth, aoclamp, 1.0) + (noise.x * (1.0 - noise.x));
    float h = (1.0 / resolution.y) / clamp(depth, aoclamp, 1.0) + (noise.y * (1.0 - noise.y));

    //float pw = 0.0;
    //float ph = 0.0;

    float ao = 0.0;

    float dl = PI * (3.0 - sqrt(5.0));
    float dz = 1.0 / float(samples);
    float l = 0.0;
    float z = 1.0 - dz / 2.0;

    for (int i = 0; i < samples; i++) {
	//if (i > samples) break;
	float r = sqrt(1.0 - z);

	//pw = cos(l) * r;
	//ph = sin(l) * r;
	ao += calAO(depth, cos(l) * r * w, sin(l) * r * h, CurrentNormal, radius);
	z -= dz;
	l += dl;
    }
    ao = 1.0 - ao / float(samples) * strength;
    return clamp(0., 1., ao);
    return ao;
}



/********************************* SSR *************************/

//vec3 V - normalized view direction
//vec3 L - normalized light direction
//float roughness - smooth (0.0) to rough (1.0)
//float F0 - fresnel term, 0.0 to 1.0
float ggx(vec3 L, float roughness, float F0) {
    vec3 V = normalize(getCameraPos() - CurrentPosition);
    float alpha = roughness * roughness;
    vec3 H = normalize(L - V);
    float dotLH = max(0.0, dot(L,H));
    float dotNH = max(0.0, dot(CurrentNormal,H));
    float dotNL = max(0.0, dot(CurrentNormal,L));
    float alphaSqr = alpha * alpha;
    float denom = dotNH * dotNH * (alphaSqr - 1.0) + 1.0;
    float D = alphaSqr / (3.141592653589793 * denom * denom);
    float F = F0 + (1.0 - F0) * pow(1.0 - dotLH, 5.0);
    float k = 0.5 * alpha;
    float k2 = k * k;
    return dotNL * D * F / (dotLH*dotLH*(1.0-k2)+k2);
}

///////////////////////////  http://roar11.com/2015/07/screen-space-glossy-reflections/ ///////////////

float cb_stride = 3.0f; // Step in horizontal or vertical pixels between samples. This is a float
// because integer math is slow on GPUs, but should be set to an integer >= 1.
float cb_strideZCutoff = 0.01f; // More distant pixels are smaller in screen space. This value tells at what point to
// start relaxing the stride to give higher quality reflections for objects far from
// the camera.

float distanceSquared(vec2 a, vec2 b) {
    a -= b;
    return dot(a, a);
}

bool intersectsDepthBuffer(float z, float minZ, float maxZ) {
    float cb_zThickness = 0.25f; // thickness to ascribe to each pixel in the depth buffer

    float depthScale = min(1.0f, z * cb_strideZCutoff);
    z += cb_zThickness + mix(0.0f, 2.0f, depthScale);
    return (maxZ >= z) && (minZ - cb_zThickness <= z);
}

void swap(inout float a, inout float b) {
    float t = a;
    a = b;
    b = t;
}

float linearDepthTexelFetch(ivec2 hitPixel) {
    // Load returns 0 for any value accessed out of bounds
    return linearizeDepth(texelFetch(gDepth, hitPixel, 0).x);
}

// Returns true if the ray hit something
bool traceScreenSpaceRay(
	// Camera-space ray origin, which must be within the view volume
	vec3 csOrig,
	// Unit length camera-space ray direction
	vec3 csDir,
	// Number between 0 and 1 for how far to bump the ray in stride units
	// to conceal banding artifacts. Not needed if stride == 1.
	float jitter,
	// Pixel coordinates of the first intersection with the scene
	out vec2 hitPixel,
	// Camera space location of the ray hit
	out vec3 hitPoint) {
    vec2 cb_depthBufferSize = vec2(1920,1080); // dimensions of the z-buffer
    float cb_maxSteps = 200; // Maximum number of iterations. Higher gives better images but may be slow.
    float cb_maxDistance = 0; // Maximum camera-space distance to trace before returning a miss.

    // Clip to the near plane
    float rayLength = ((csOrig.z + csDir.z * cb_maxDistance) < zNear) ?
	(zNear - csOrig.z) / csDir.z : cb_maxDistance;
    vec3 csEndPoint = csOrig + csDir * rayLength;

    mat4 viewToTextureSpaceMatrix = mat4(0.5,  0.0,  0.0,  0.0,
	    				0.0, -0.5,  0.0,  0.0, 
	    				0.0,  0.0,  1.0,  0.0, 
	    				0.5,  0.5,  0.0,  1.0);
    vec4 H0 = viewToTextureSpaceMatrix * vec4(csOrig, 1.0f);
    H0.xy *= cb_depthBufferSize;
    vec4 H1 = viewToTextureSpaceMatrix * vec4(csEndPoint, 1.0f);
    H1.xy *= cb_depthBufferSize;
    float k0 = 1.0f / H0.w;
    float k1 = 1.0f / H1.w;

    // The interpolated homogeneous version of the camera-space points
    vec3 Q0 = csOrig * k0;
    vec3 Q1 = csEndPoint * k1;

    // Screen-space endpoints
    vec2 P0 = H0.xy * k0;
    vec2 P1 = H1.xy * k1;

    // If the line is degenerate, make it cover at least one pixel
    // to avoid handling zero-pixel extent as a special case later
    if ((distanceSquared(P0, P1) < 0.0001f)) {
	P1 += vec2(0.01f, 0.01f);
    }
    vec2 delta = P1 - P0;

    // Permute so that the primary iteration is in x to collapse
    // all quadrant-specific DDA cases later
    bool permute = false;
    if(abs(delta.x) < abs(delta.y)) {
	// This is a more-vertical line
	permute = true;
	delta = delta.yx;
	P0 = P0.yx;
	P1 = P1.yx;
    }

    float stepDir = sign(delta.x);
    float invdx = stepDir / delta.x;

    // Track the derivatives of Q and k
    vec3 dQ = (Q1 - Q0) * invdx;
    float dk = (k1 - k0) * invdx;
    vec2 dP = vec2(stepDir, delta.y * invdx);

    // Scale derivatives by the desired pixel stride and then
    // offset the starting values by the jitter fraction
    float strideScale = 1.0f - min(1.0f, csOrig.z * cb_strideZCutoff);
    float stride = 1.0f + strideScale * cb_stride;
    dP *= stride;
    dQ *= stride;
    dk *= stride;

    P0 += dP * jitter;
    Q0 += dQ * jitter;
    k0 += dk * jitter;

    // Slide P from P0 to P1, (now-homogeneous) Q from Q0 to Q1, k from k0 to k1
    vec4 PQk = vec4(P0, Q0.z, k0);
    vec4 dPQk = vec4(dP, dQ.z, dk);
    vec3 Q = Q0;
    // Adjust end condition for iteration direction
    float end = P1.x * stepDir;

    float stepCount = 0.0f;
    float prevZMaxEstimate = csOrig.z;
    float rayZMin = prevZMaxEstimate;
    float rayZMax = prevZMaxEstimate;
    float sceneZMax = rayZMax + 100.0f;
    for(;
	    ((PQk.x * stepDir) <= end) && (stepCount < cb_maxSteps) &&
	    !intersectsDepthBuffer(sceneZMax, rayZMin, rayZMax) &&
	    (sceneZMax != 0.0f);
	    ++stepCount) {
	rayZMin = prevZMaxEstimate;
	rayZMax = (dPQk.z * 0.5f + PQk.z) / (dPQk.w * 0.5f + PQk.w);
	prevZMaxEstimate = rayZMax;
	if(rayZMin > rayZMax) {
	    swap(rayZMin, rayZMax);
	}

	hitPixel = permute ? PQk.yx : PQk.xy;
	// You may need hitPixel.y = depthBufferSize.y - hitPixel.y; here if your vertical axis
	// is different than ours in screen space
	sceneZMax = linearDepthTexelFetch(ivec2(hitPixel));

	PQk += dPQk;
    }

    // Advance Q based on the number of steps
    Q.xy += dQ.xy * stepCount;
    hitPoint = Q * (1.0f / PQk.w);
    return intersectsDepthBuffer(sceneZMax, rayZMin, rayZMax);
}

vec3 raytrace1() {
    //ivec3 loadIndices = ivec3(pIn.posH.xy, 0);
    //vec3 normalVS = normalBuffer.Load(loadIndices).xyz; // = CurrentNormal;

    //float depth = depthBuffer.Load(loadIndices).r; // = CurrentDepth
    vec3 rayOriginVS = normalize(CurrentPosition - getCameraPos()) * CurrentDepth;

    vec3 toPositionVS = normalize(rayOriginVS);
    vec3 rayDirectionVS = normalize(reflect(toPositionVS, CurrentNormal));

    // output rDotV to the alpha channel for use in determining how much to fade the ray
    float rDotV = dot(rayDirectionVS, toPositionVS);

    // out parameters
    vec2 hitPixel = vec2(0.0f, 0.0f);
    vec3 hitPoint = vec3(0.0f, 0.0f, 0.0f);

    vec2 j = TexCoords * resolution;
    float jitter = cb_stride > 1.0f ? float(int(j.x + j.y) & 1) * 0.5f : 0.0f;

    // perform ray tracing - true if hit found, false otherwise
    bool intersection = traceScreenSpaceRay(rayOriginVS, rayDirectionVS, jitter, hitPixel, hitPoint);

    float depth = texelFetch(gDepth, ivec2(hitPixel), 0).x;
    //depth = depthBuffer.Load(ivec3(hitPixel, 0)).r;

    // move hit pixel from pixel position to UVs
    //hitPixel *= 1 / resolution;
    //if(hitPixel.x > 1.0f || hitPixel.x < 0.0f || hitPixel.y > 1.0f || hitPixel.y < 0.0f) {
    //    intersection = false;
    //}
    //return vec3(hitPixel, depth) * (intersection ? 1.0f : 0.0f);

    if (intersection) {
	return texelFetch(gNormal, ivec2(hitPixel), 0).xyz;
    } else {
	vec3 (0.0f);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

vec3 naiveRaymarch(in vec3 reflectionVector, in sampler2D tex) {
    int maxComplexity = int(mix(8, 4, 1 - CurrentDepth));
    const float baseThreshold = 0.9;
    const float targetThreshold = 0.8;
    float samplingOffset = 1;
    int complexity = 0;
    vec2 stepDir = normalize(reflectionVector.xy);
    float threshold = baseThreshold;

    reflectionVector = (inverse(uView) * vec4(reflectionVector, 0.0f)).xyz;
    // Current sampling position is at current fragment
    //mat4 toViewSpace = transpose(inverse(uView));
    vec2 sampledPosition = TexCoords;
    //vec3 startPos = (toViewSpace * vec4(texture(gPosition, TexCoords).xyz, 1.0f)).xyz;
    vec3 startPos = CurrentPosition;
    vec3 sampledViewPos = startPos;
    float test = 0.000;

    vec2 stepSizeMax = .0f / resolution;
    vec2 stepSizemin = 32.0f / resolution;
    vec2 stepSize = stepSizemin;
    while((sampledPosition.x <= 1.0 && sampledPosition.x >= 0.0 && sampledPosition.y <= 1.0 && sampledPosition.y >= 0.0) && (complexity < maxComplexity) && (test < threshold)) {
	sampledPosition += stepDir * stepSize;
	sampledViewPos = texture2D(gPosition, sampledPosition, samplingOffset).xyz;
	test = dot(normalize(sampledViewPos - startPos), reflectionVector);
	threshold = mix(baseThreshold, targetThreshold, float(complexity) / float(maxComplexity));
	float d = distance(TexCoords, sampledPosition) / 0.1;
	stepSize *= 2;
	samplingOffset = mix(1, 6, d); // really cheap fake "cone tracing" using mipmap filtering
	++complexity;
    }

    if (sampledPosition.x > 1.0 || sampledPosition.x <= 0.0 || sampledPosition.y > 1.0 && sampledPosition.y < 0.0) {
    }
    float facing = dot(texture2D(gNormal, sampledPosition, samplingOffset).xyz * 2.0f - 1.0f, reflectionVector);
    //return -facing.xxx;
    if ((facing < 0.0f) && (test > threshold)) {
	return texture2D(tex, sampledPosition, samplingOffset).xyz;
    } else {
	vec3 (0.0f);
    }
}

// TODO configurable GGX
vec3 raytrace(in vec3 reflectionVector) {
    return naiveRaymarch(reflectionVector, gNormal);
}

vec3 SSR(float fresnel) {
    float currDepth = linearizeDepth(texture(gDepth, TexCoords).x);
    vec3 reflectionVector = reflect(normalize((uView * texture2D(gPosition, TexCoords, 1)).xyz), CurrentNormal);
    if (isRayTowardCamera(reflectionVector.xyz) || CurrentDepth > 0.17 || fresnel < 0.03) {
	return vec3(0.0f);
    } else {
	return raytrace(reflectionVector.xyz) * (1 - CurrentDepth / .4);
    }
}

/****************************** Compositor *******************/

vec3 pbrBlending(vec3 Kdiff, vec3 Kspec, vec3 Kbase, float metallic) {
    float scRange = smoothstep(0.2, 0.45, metallic);
    vec3  dielectric = Kdiff + Kspec;
    vec3  metal = Kspec * Kbase;
    return mix(dielectric, metal, scRange);
}


void main() { 
    vec2 distCoord = TexCoords * 2.0f;

    CurrentNormal = normalize(texture2D(gNormal, TexCoords).xyz * 2.0f - 1.0f);
    CurrentNormalWorldSpace = (inverse(uView) * vec4(CurrentNormal,0.0f)).xyz;
    CurrentPosition = texture2D(gPosition, TexCoords).xyz;
    CurrentDepthNotLinearized = texture2D(gDepth, TexCoords).x;
    CurrentDepth = linearizeDepth(CurrentDepthNotLinearized);
    
    //outColour = cn + cp + ca + cd;
    //outColour = SSR();
    //outColour = mix(SSR(), texture(gNormal, TexCoords).xyz, timeBounce(800));

    vec3 Light = vec3(0.0f, 12.0f, 11.0f);
    outColour = normalize(abs(CurrentNormalWorldSpace)) * ggx(Light, 0.1f, fresnel(1.4)).xxx;
    //outColour = fresnel(1.4).xxx;

    //outColour = SSR(fresnel(1.4));
    //outColour = raytrace1();
    //outColour = vec3(ssao(10, 1));
    //outColour = texture2D(gNormal, TexCoords, 5).xyz;
    //float f = fresnel(1.4);
    //outColour = mix(texture2D(gNormal, TexCoords, 0).xyz, SSR(f), f) * ssao(10, 1);
    //outColour = (inverse(uView) * vec4(texture(gPosition, TexCoords).xyz, 1.0f)).xyz - getCameraPos();
    //outColour = vec3(fresnel());
}
