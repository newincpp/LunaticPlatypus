#version 430 core

layout (location = 0) uniform float uTime;
layout (location = 1) uniform mat4 uMeshTransform;
layout (location = 2) uniform mat4 uView;
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

// -------------

/*
SSAO GLSL shader v1.2
assembled by Martins Upitis (martinsh) (devlog-martinsh.blogspot.com)
original technique is made by Arkano22 (www.gamedev.net/topic/550699-ssao-no-halo-artifacts/)
changelog:
1.2 - added fog calculation to mask AO. Minor fixes.
1.1 - added spiral sampling method from here:
(http://www.cgafaq.info/wiki/Evenly_distributed_points_on_sphere)
*/
//uniform sampler2D bgl_RenderedTexture;

#define PI    3.14159265

float linearizeDepth(float z) {
    //float near = 0.1;  // the near plane
    //float far = 100.0; // the far plane
    return (2.0 * zNear) / (zFar + zNear - z * (zFar - zNear));
}

vec3 getCameraPos() {
    return inverse(uView)[3].xyz;
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
    radius += sin(uTime / 400) /5;
    float aoclamp = 0.125; //depth clamp - reduces haloing at screen edges
    float strength = 1.2;

    vec2 noise = randomFromUv(TexCoords);
    float depth = readDepth(TexCoords);
    vec3 normal = texture2D(gNormal, TexCoords).xyz;

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
	ao += calAO(depth, cos(l) * r * w, sin(l) * r * h, normal, radius);
	z -= dz;
	l += dl;
    }
    ao = 1.0 - ao / float(samples) * strength;
    return clamp(0., 1., ao);
}



/********************************* SSR *************************/

vec3 raytrace0(in vec3 reflectionVector, in float startDepth, in sampler2D tex) {
    vec3 color = vec3(0.0f, 0.0f, 0.4f);
    float stepSize = 0.01f; 
    int steps = 0;

    float size = length(reflectionVector.xy);
    reflectionVector = normalize(reflectionVector/size) * stepSize;

    // Current sampling position is at current fragment
    vec2 sampledPosition = TexCoords.xy;
    // Current depth at current fragment
    float currentDepth = startDepth;
    // The sampled depth at the current sampling position
    float sampledDepth = linearizeDepth( texture(gDepth, sampledPosition).x);

    // Raytrace as long as in texture space of depth buffer (between 0 and 1)
    while(sampledPosition.x <= 1.0 && sampledPosition.x >= 0.0 &&
	    sampledPosition.y <= 1.0 && sampledPosition.y >= 0.0) {
	// Update sampling position by adding reflection vector's xy and y components
	sampledPosition = sampledPosition + reflectionVector.xy;
	// Updating depth values
	currentDepth = currentDepth + reflectionVector.z * startDepth;
	float sampledDepth = linearizeDepth( texture(gDepth, sampledPosition).z );

	// If current depth is greater than sampled depth of depth buffer, intersection is found
	if(currentDepth > sampledDepth) {
	    // Delta is for stop the raytracing after the first intersection is found
	    // Not using delta will create "repeating artifacts"
	    float delta = (currentDepth - sampledDepth);
	    if(delta < 0.003f ) {
		color = texture(tex, sampledPosition).rgb;
		return color;
	    }
	}
	if (steps > 20) {
	    return color;
	}
	steps++;
    }
    return color;
}




// By Morgan McGuire and Michael Mara at Williams College 2014
// Released as open source under the BSD 2-Clause License
// http://opensource.org/licenses/BSD-2-Clause
float distanceSquared(vec2 a, vec2 b) { a -= b; return dot(a, a); }

// Returns true if the ray hit something
bool McGuireTraceScreenSpaceRay1(vec3 csOrig, vec3 csDir, mat4x4 proj, sampler2D csZBuffer, vec2 csZBufferSize, float zThickness, float nearPlaneZ, float stride, float jitter, const float maxSteps, float maxDistance, out vec2 hitPixel, out vec3 hitPoint, out float complexity) {
    float rayLength = ((csOrig.z + csDir.z * maxDistance) > nearPlaneZ) ?
	(nearPlaneZ - csOrig.z) / csDir.z : maxDistance;
    vec3 csEndPoint = csOrig + csDir * rayLength;

    // Project into homogeneous clip space
    vec4 H0 = proj * vec4(csOrig, 1.0);
    vec4 H1 = proj * vec4(csEndPoint, 1.0);
    float k0 = 1.0 / H0.w, k1 = 1.0 / H1.w;

    // The interpolated homogeneous version of the camera-space points
    vec3 Q0 = csOrig * k0, Q1 = csEndPoint * k1;

    // Screen-space endpoints
    vec2 P0 = H0.xy * k0, P1 = H1.xy * k1;

    // If the line is degenerate, make it cover at least one pixel
    // to avoid handling zero-pixel extent as a special case later
    P1 += vec2((distanceSquared(P0, P1) < 0.0001) ? 0.01 : 0.0);
    vec2 delta = P1 - P0;

    // Permute so that the primary iteration is in x to collapse
    // all quadrant-specific DDA cases later
    bool permute = false;
    if (abs(delta.x) < abs(delta.y)) {
	// This is a more-vertical line
	permute = true; delta = delta.yx; P0 = P0.yx; P1 = P1.yx;
    }

    float stepDir = sign(delta.x);
    float invdx = stepDir / delta.x;

    // Track the derivatives of Q and k
    vec3  dQ = (Q1 - Q0) * invdx;
    float dk = (k1 - k0) * invdx;
    vec2  dP = vec2(stepDir, delta.y * invdx);

    // Scale derivatives by the desired pixel stride and then
    // offset the starting values by the jitter fraction
    dP *= stride; dQ *= stride; dk *= stride;
    P0 += dP * jitter; Q0 += dQ * jitter; k0 += dk * jitter;

    // Slide P from P0 to P1, (now-homogeneous) Q from Q0 to Q1, k from k0 to k1
    vec3 Q = Q0;

    // Adjust end condition for iteration direction
    float  end = P1.x * stepDir;

    float k = k0, stepCount = 0.0, prevZMaxEstimate = csOrig.z;
    float rayZMin = prevZMaxEstimate, rayZMax = prevZMaxEstimate;
    float sceneZMax = rayZMax + 100;
    for (vec2 P = P0;
	    ((P.x * stepDir) <= end) && (stepCount < maxSteps) &&
	    ((rayZMax < sceneZMax - zThickness) || (rayZMin > sceneZMax)) &&
	    (sceneZMax != 0);
	    P += dP, Q.z += dQ.z, k += dk, ++stepCount) {

	rayZMin = prevZMaxEstimate;
	rayZMax = (dQ.z * 0.5 + Q.z) / (dk * 0.5 + k);
	prevZMaxEstimate = rayZMax;
	if (rayZMin > rayZMax) {
	    float t = rayZMin;
	    rayZMin = rayZMax;
	    rayZMax = t;
	}

	hitPixel = permute ? P.yx : P;
	// You may need hitPixel.y = csZBufferSize.y - hitPixel.y; here if your vertical axis
	// is different than ours in screen space
	sceneZMax = texelFetch(csZBuffer, ivec2(hitPixel), 0).x;
	complexity += 1;
    }

    // Advance Q based on the number of steps
    Q.xy += dQ.xy * stepCount;
    hitPoint = Q * (1.0 / k);
    return (rayZMax >= sceneZMax - zThickness) && (rayZMin < sceneZMax);
}

vec3 raytrace1(in vec3 reflectionVector, in sampler2D tex) {
    vec2 hitPixel = vec2(0.0f);
    vec3 hitPoint = vec3(0.0f);
    float complexity = 0.;
    mat4 s = mat4(1.0f);
    s[0][0] = resolution.x/2;
    s[1][1] = resolution.y/2;
    s[3][0] = resolution.x/2;
    s[3][1] = resolution.y/2;
    vec2 uv2 = TexCoords * vec2(1920, 1080); 
    float jitter = mod((uv2.x + uv2.y) * 0.25, 1.0);
    const float maxSteps= 8.0f;
    const float maxDistance = 32;
    float zThickness = 24;
    			//( vec3 csOrig, vec3 csDir, mat4x4 proj, sampler2D csZBuffer, vec2 csZBufferSize, float zThickness, float nearPlaneZ, float stride, float jitter, const float maxSteps, float maxDistance, out vec2 hitPixel, out vec3 hitPoint, float iterations)
    bool hit = McGuireTraceScreenSpaceRay1((uView * texture(gPosition, TexCoords)).xyz, normalize(reflectionVector), s * uProjection, gDepth, resolution, zThickness, zNear, 1.0f, jitter, maxSteps, maxDistance, hitPixel, hitPoint, complexity);
    //return vec3(complexity / maxSteps);
    //return vec3(hitPixel / vec2(1920, 1080), 0.);
    if (hit) {
	//return vec3(1.0);
	//return vec3(TexCoords, 0.);
	return texture(tex, hitPixel/vec2(1920,1080)).xyz;
    } else {
        return vec3(.0f, .0f, 0.0);
    }
}

vec3 raytrace(in vec3 reflectionVector, in sampler2D tex) {
    return raytrace1(reflectionVector, tex);
    //return raytrace0(reflectionVector, linearizeDepth(texture(gDepth, TexCoords).x), tex);
}

vec3 SSR() {
    vec3 normal = normalize(texture(gNormal, TexCoords).xyz * 2.0f - 1.0f);
    float currDepth = linearizeDepth(texture(gDepth, TexCoords).x);
    vec3 reflectionVector = reflect(normalize((uView * texture(gPosition, TexCoords)).xyz), normal);
    //return raytrace(-reflectionVector.xyz, currDepth, gNormal);
    return raytrace(reflectionVector.xyz, gNormal);
}

/****************************** Compositor *******************/

void main() { 
    vec2 distCoord = TexCoords * 2.0f;
    vec3 cp = texture(gPosition, distCoord).rgb;
    vec3 cn = texture(gNormal, distCoord - vec2(1.0f, 0.0f)).rgb;
    vec3 ca = texture(gAlbedoSpec, distCoord - vec2(0.0f, 1.0f)).rgb;
    float z = texture(gDepth, distCoord - vec2(1.0f, 1.0f)).r;
    float near = 0.1;                                // the near plane
    float far = 100.0;                               // the far plane
    float c = (2.0 * near) / (far + near - z * (far - near));  // convert to linear values 
    vec3 cd = vec3(c);
    //outColour = cn + cp + ca + cd;
    //outColour = mix(SSR(), texture(gNormal, TexCoords).xyz, 0.3) * ssao(12, 1.5f);
    outColour = clamp(SSR(), 0., 1.) * ssao(12, 1.5f);
}
