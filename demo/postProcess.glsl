#version 430 core

layout (location = 0) uniform float uTime;
layout (location = 1) uniform mat4 uMeshTransform;
layout(location = 2) uniform mat4 uView;
layout(location = 3) uniform mat4 uProjection;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDepth;

uniform layout(binding=1, rgba16f) image2D uFractalTexture; // float
//uniform layout(binding=1, rgba16ui) uimage2D uFractalTexture; // int

in vec2 TexCoords;

out vec3 outColour;

vec2 resolution = vec2(1920, 1024);
float zNear = 0.1;
float zFar = 300.0;

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
    return ao;
}


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
    //outColour = vec3(ssaoAlone());
    //ssaoI();
    //outColour = vec3(ssaoAlone());
    //ivec2 plop = ivec2(TexCoords.x * 1920, TexCoords.y * 1080);
    ivec2 plop = ivec2(distCoord * imageSize(uFractalTexture));
    outColour = imageLoad(uFractalTexture, plop).xyz + cn + ca + c;
    //imageStore(uFractalTexture, plop, uvec4(0.0));
    //outColour = abs(texture(gNormal, TexCoords).xyz) * ssao();
}
