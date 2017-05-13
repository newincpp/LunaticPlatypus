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
vec3 CurrentAlbedo;
vec3 CurrentNormal;
vec3 CurrentNormalWorldSpace;
vec3 CurrentNormalWorld;
vec3 CurrentPosition;
float CurrentDepth;
float CurrentDepthNotLinearized;


struct Light {
    vec3 position;
    vec3 colour;
};


Light debugDefaultLight[4];
//Light debugDefaultLight;


#define PI    3.14159265
#define OREN_NAYAR

/* http://filmicworlds.com/blog/filmic-tonemapping-operators/
float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

float3 Uncharted2Tonemap(float3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}
*/

float linearizeDepth(float z) {
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

vec3 fresnel(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
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

vec3 naiveRaymarch(in vec3 reflectionVector, in sampler2D tex, vec3 diffuseColour) {
    int maxComplexity = int(mix(16, 3, 1 - CurrentDepth));
    const float baseThreshold = 0.7;
    const float targetThreshold = 0.9;
    float samplingOffset = 0;
    int complexity = 0;
    vec2 stepDir = normalize(reflectionVector.xy);
    float threshold = baseThreshold;

    // reflectionVector = (inverse(uView) * vec4(reflectionVector, 0.0f)).xyz;

    // Current sampling position is at current fragment
    // mat4 toViewSpace = transpose(inverse(uView));
    vec2 sampledPosition = TexCoords;
    vec3 sampledViewPos = CurrentPosition;
    float test = 0.000;

    vec2 stepSize = resolution / 100;
    while((sampledPosition.x <= 1.0 && sampledPosition.x >= 0.0 && sampledPosition.y <= 1.0 && sampledPosition.y >= 0.0) && (complexity < maxComplexity) && (test < threshold)) {
	sampledPosition += stepDir / stepSize;
	sampledViewPos = texture2D(gPosition, sampledPosition, samplingOffset).xyz;
	test = dot(normalize(sampledViewPos - CurrentPosition), reflectionVector);
	threshold = mix(baseThreshold, targetThreshold, float(complexity) / float(maxComplexity));
	stepSize *= 4;
	//stepSize *= stepSize;
	++complexity;
    }

    float facing = dot(texture2D(gNormal, sampledPosition, samplingOffset).xyz * 2.0f - 1.0f, reflectionVector);
    float d = clamp(0.0f, 1.0f, distance(CurrentPosition, sampledViewPos) / 100.0f);
    samplingOffset = mix(1.0, 6.0, d); // really cheap fake "cone tracing" using mipmap filtering
    if ((facing < 0.0f) && (test > threshold)) {
	return mix(texture2D(tex, sampledPosition, samplingOffset).xyz, diffuseColour, d);
    } else {
	return diffuseColour;
    }
}

vec3 raytrace(in vec3 reflectionVector, vec3 diffuseColour) {
    return naiveRaymarch(reflectionVector, gAlbedoSpec, diffuseColour);
}

vec3 SSR(float fresnel, vec3 diffuseColour) {
    //vec3 reflectionVector = reflect(normalize((uView * texture2D(gPosition, TexCoords, 1)).xyz), CurrentNormal);
    vec3 reflectionVector = reflect(normalize(CurrentPosition - getCameraPos()), CurrentNormalWorldSpace);

    if (isRayTowardCamera(reflectionVector.xyz) || CurrentDepth > 0.47 || fresnel < 0.02) {
	return diffuseColour;
    } else {
	return raytrace(reflectionVector.xyz, diffuseColour);
	//return raytrace(reflectionVector.xyz) * (1 - CurrentDepth / .4);
    }
}


/**************************** Physically Based Lighting **************************************/

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 orenNayarDiffuse(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness, vec3 albedo) {
  float NdotL = dot(lightDirection, surfaceNormal);
  float NdotV = dot(surfaceNormal, viewDirection);

  float s = dot(lightDirection, viewDirection) - NdotL * NdotV;
  float t = mix(1.0, max(NdotL, NdotV), step(0.0, s));

  float sigma2 = roughness * roughness;
  vec3 A = 1.0 + sigma2 * (albedo / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
  float B = 0.45 * sigma2 / (sigma2 + 0.09);

  return albedo * max(0.0, NdotL) * (A + B * s / t) / PI;
}

vec3 cookTorrance(vec3 normal, vec3 lightDir, vec3 cameraDir, float D, vec3 F, float G) {
    vec3 nominator = D * F * G;
    float denominator = 4 * max(dot(normal, cameraDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.001; 
    return nominator / denominator;
}

vec3 pbrDirectIllumination(float roughness, float metallic, float ao) {
    vec3 N = CurrentNormal;
    vec3 V = normalize(getCameraPos() - CurrentPosition);

    vec3 Lo = vec3(0.0);

    for(int i = 0; i < 4; ++i) { // Loop over Lights
	// direct radiance
	float distance = length(debugDefaultLight[i].position - CurrentPosition);
	float attenuation = 1.0 / (distance * distance);
	if (attenuation > 0.001) {
	    vec3 radiance = debugDefaultLight[i].colour * attenuation; 

	    vec3 L = normalize(debugDefaultLight[i].position - CurrentPosition);
	    vec3 H = normalize(V + L);


	    vec3 F = fresnel(max(dot(H, V), 0.0), mix(vec3(0.04), CurrentAlbedo, metallic));

	    // Bidirectional reflectance distribution function (BRDF)
#ifdef OREN_NAYAR
	    vec3 brdf = orenNayarDiffuse(L, V, N, roughness, CurrentAlbedo);
#else
	    vec3 brdf = cookTorrance(N, L, V, DistributionGGX(N, H, roughness), F, GeometrySmith(N, V, L, roughness));
#endif

	    vec3 kS = F;
	    vec3 kD = vec3(1.0) - kS;
	    kD *= 1.0 - metallic;

	    float NdotL = max(dot(N, L), 0.0);        
	    Lo += (kD * CurrentAlbedo / PI + brdf) * radiance * NdotL;
	}
    }


    // TODO inacurate ambient term
    vec3 ambient = vec3(0.00) * CurrentAlbedo * ao;
    vec3 color = ambient + Lo; 


    // gamma correction
    color = color / (color + vec3(1.0));
    return pow(color, vec3(1.0/2.2)); 
}


/****************************** Compositor *******************/


vec3 VSPositionFromDepth(vec2 vTexCoord) {
    float z = texture2D(gDepth, vTexCoord).x;
    float x = vTexCoord.x * 2 - 1;
    float y = (1 - vTexCoord.y) * 2 - 1;
    vec4 vProjectedPos = vec4(x, y, z, 1.0f);
    vec4 vPositionVS = inverse(uProjection) * vProjectedPos;
    vec3 p = vPositionVS.xyz / vPositionVS.w;
    return p;
    //if (timeBounce(600) < 0.5) {
    //} else {
    //    return (inverse(uView) * vec4(p, 0.0f)).xyz;
    //}
}

void main() { 
    CurrentNormal = normalize(texture2D(gNormal, TexCoords).xyz * 2.0f - 1.0f);
    CurrentNormalWorldSpace = (inverse(uView) * vec4(CurrentNormal,0.0f)).xyz;
    CurrentPosition = texture2D(gPosition, TexCoords).xyz;
    CurrentDepthNotLinearized = texture2D(gDepth, TexCoords).x;
    CurrentDepth = linearizeDepth(CurrentDepthNotLinearized);
    CurrentAlbedo = texture2D(gAlbedoSpec, TexCoords).xyz;

    float metallicness = 1.;
    float roughness = 1.00;

    debugDefaultLight[0] = Light(mix(vec3(-17,2,-7), vec3(4,2,0), timeBounce(900)), vec3(4.8, 4.8, 4.75));
    debugDefaultLight[1] = Light(-debugDefaultLight[0].position, debugDefaultLight[0].colour);
    debugDefaultLight[2] = Light(mix(vec3(15, (5 * sin(uTime / 900)) + 8, 8), vec3(-17, (5 * sin(uTime / 400)) + 8, 8.5), timeBounce(1400)), vec3(9.8, 9.8, 9.75));
    debugDefaultLight[3] = Light(mix(vec3(-1.5, 5, -2), vec3(-1.5, 21, 7), timeBounce(900)), vec3(12.8, 12.8, 12.75));
    //outColour = pbrDirectIllumination(roughness, metallicness, ssao(6, 1));
    //outColour = pbrDirectIllumination(roughness, metallicness, 1.0f);
    float f = clamp(fresnel(2.5), 0.0, 1.0);
    //outColour = mix(outColour, SSR(f, outColour), f); // ad-hoc way to add SSR while I didn't implement IBL
    //outColour = SSR(f, vec3(0.0,0.05,0.05));
    //outColour = texture2D(gAlbedoSpec, TexCoords, 6 * timeBounce(600)).xyz;
    outColour = CurrentNormalWorldSpace;
}

