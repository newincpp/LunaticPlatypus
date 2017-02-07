#version 430 core

layout (location = 0) uniform float uTime;
layout (location = 1) uniform mat4 uMeshTransform;
layout(location = 2) uniform mat4 uView;
layout(location = 3) uniform mat4 uProjection;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDepth;

uniform layout(binding=1, rgba16f) image2D uFractalTexture;
//uniform layout(binding=1, rgba16ui) uimage2D uFractalTexture;

in vec2 TexCoords;

out vec3 outColour;

vec2 resolution = vec2(1920, 1024);
float zNear = 0.1;
float zFar = 300.0;
float strength = 1.5;

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

float width = resolution.x; //texture width
float height = resolution.y; //texture height


//------------------------------------------
//general stuff

//user variables
int samples = 64; //ao sample count //64
float radius = 5.0; //ao radius //5.0

float aoclamp = 0.125; //depth clamp - reduces haloing at screen edges
bool noise = true; //use noise instead of pattern for sample dithering
float noiseamount = 0.0002; //dithering amount

float diffarea = 0.3; //self-shadowing reduction
float gdisplace = 0.4; //gauss bell center //0.4

bool mist = false; //use mist?
float miststart = 0.0; //mist start
float mistend = zFar; //mist end

bool onlyAO = false; //use only ambient occlusion pass?
float lumInfluence = 0.7; //how much luminance affects occlusion

//--------------------------------------------------------

vec2 randomFromUv(vec2 coord) { //generating noise/pattern texture for dithering
	float noiseX = ((fract(1.0 - coord.s * (width / 2.0)) * 0.25) + (fract(coord.t * (height / 2.0)) * 0.75)) * 2.0 - 1.0;
	float noiseY = ((fract(1.0 - coord.s * (width / 2.0)) * 0.75) + (fract(coord.t * (height / 2.0)) * 0.25)) * 2.0 - 1.0;

	//if (noise) {
	//	noiseX = clamp(fract(sin(dot(coord, vec2(12.9898, 78.233))) * 43758.5453), 0.0, 1.0) * 2.0 - 1.0;
	//	noiseY = clamp(fract(sin(dot(coord, vec2(12.9898, 78.233) * 2.0)) * 43758.5453), 0.0, 1.0) * 2.0 - 1.0;
	//}
	return vec2(noiseX, noiseY) * noiseamount;
}

float doMist() {
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

int compareDepthsFar(float depth1, float depth2) {
	//float garea = 2.0; //gauss bell width
	float diff = (depth1 - depth2) * 100.0; //depth difference (0-100)
	//reduce left bell width to avoid self-shadowing
	return diff < gdisplace ? 0 : 1;
	//if (diff<gdisplace) {
	//	return 0;
	//} else {
	//	return 1;
	//}
}

float compareDepths(float depth1, float depth2) {
	float garea = 2.0; //gauss bell width
	float diff = (depth1 - depth2) * 100.0; //depth difference (0-100)
	//reduce left bell width to avoid self-shadowing
	garea = diff < gdisplace ? diffarea : garea;
	//if (diff < gdisplace) {
	//	garea = diffarea;
	//}

	return pow(2.7182, -2.0 * pow((diff - gdisplace) / garea, 2.0));
}

float directionMultiplier(float coord, float texCoord, float normal, float cn) {
    return coord - texCoord < 0 ? (normal - cn > 0 ? 0.0 : 1.0) : normal - cn > 0 ? 1.0 : 0.0;
}

float dotDirection(vec3 a, vec3 b) {
    return sign(a.x * -b.y + a.y * b.x);
}

//9ms
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

float calAO(float depth, float dw, float dh, vec3 normal) {
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
	temp = compareDepths(depth, cd) * normalStuff(normal, coord);
	//DEPTH EXTRAPOLATION: 5ms
	int far = compareDepthsFar(depth, cd);
	if (far > 0) {
	    temp += (1.0 - temp) * compareDepths(readDepth(coord2), depth) * normalStuff(normal, coord2);
	}

	return temp;
}

float ssaoAlone() {
	vec2 noise = randomFromUv(TexCoords);
	float depth = readDepth(TexCoords);
	vec3 normal = texture2D(gNormal, TexCoords).xyz;

	float w = (1.0 / width) / clamp(depth, aoclamp, 1.0) + (noise.x * (1.0 - noise.x));
	float h = (1.0 / height) / clamp(depth, aoclamp, 1.0) + (noise.y * (1.0 - noise.y));

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
		ao += calAO(depth, cos(l) * r * w, sin(l) * r * h, normal);
		z -= dz;
		l += dl;
	}

	//ao /= float(samples);
	//ao *= strength;
	ao = 1.0 - ao / float(samples) * strength;

	//ao = mist ? mix(ao, 1.0, doMist()) : ao;

	/*
	   vec3 color = texture2D(bgl_RenderedTexture, TexCoords).rgb;
	   vec3 lumcoeff = vec3(0.299, 0.587, 0.114);
	   float lum = dot(color.rgb, lumcoeff);
	   vec3 luminance = vec3(lum, lum, lum);
	   vec3 final = vec3(color * mix(vec3(ao), vec3(1.0), luminance * lumInfluence));//mix(color*ao, white, luminance)
	   if (onlyAO) {
	   final = vec3(mix(vec3(ao), vec3(1.0), luminance * lumInfluence)); //ambient occlusion only
	   }
	 */
	//vec3 final = vec3(depth / 1.0);
	//vec3 final = vec3(depth);

	//gl_FragColor = vec4(final, 1.0);
	return ao;

}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec2 Rand2D_bad() {
    return vec2(rand(TexCoords), rand(gl_FragCoord.xy/1000.0));
}

vec3 permute(vec3 x0, vec3 p) { 
    vec3 x1 = mod(x0 * p.y, p.x);
    return floor(mod((x1 + p.z) * x0, p.x));
}

float simplexNoise2(vec2 v) {
    const vec4 pParam = vec4( 17.0*17.0, 34.0, 1.0, 7.0);
    const vec2 C = vec2(0.211324865405187134, // (3.0-sqrt(3.0))/6.;
	    0.366025403784438597); // 0.5*(sqrt(3.0)-1.);
    const vec3 D = vec3(0., 0.5, 2.0) * 3.14159265358979312;
    // First corner
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v - i + dot(i, C.xx);

    // Other corners
    vec2 i1  =  (x0.x > x0.y) ? vec2(1.,0.) : vec2(0.,1.) ;

    //  x0 = x0 - 0. + 0. * C
    vec2 x1 = x0 - i1 + 1. * C.xx ;
    vec2 x2 = x0 - 1. + 2. * C.xx ;

    // Permutations
    i = mod(i, pParam.x);
    vec3 p = permute(permute( i.y + vec3(0., i1.y, 1. ), pParam.xyz) + i.x + vec3(0., i1.x, 1. ), pParam.xyz);

    vec3 phi = D.z * mod(p,pParam.w) /pParam.w ;
    vec4 a0 = sin(phi.xxyy+D.xyxy);
    vec2 a1 = sin(phi.zz  +D.xy);
    vec3 g = vec3(dot(a0.xy, x0), dot(a0.zw, x1), dot(a1.xy, x2));

    // mix
    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x1,x1), dot(x2,x2)), 0.);
    m = m * m ;
    return 1.66666* 70.*dot(m * m, g);
}

vec3 randomVec(vec2 uv) {
    return vec3(mod(rand(uv), 2.0) - 1.0,
		mod(rand(uv.yx), 2.0) - 1.0,
		mod(rand(uv + 0.1), 1.0));
}

float ssaoI() {
    float near = 0.1;                                // the near plane
    float far = 100.0;                               // the far plane
    //float c = (2.0 * near) / (far + near - z * (far - near));  // convert to linear values 
    vec4 eyeView = vec4(1.0) * uView;
    float zbu = (2.0 * near) / (far + near - texture2D(gDepth, TexCoords.xy).x * (far - near));
    vec3 ep = (uView * texture2D(gPosition, TexCoords)).xyz;
    //vec3 ep = zbu * eyeView.xyz / eyeView.z;
    vec3 normal = texture2D(gNormal, TexCoords.xy).xyz;
    vec3 pl = randomVec(TexCoords * dot(vec3(0.0, 0.0, 1.0), normal));//texture2D(tex1, TexCoords.xy * fres.xy);
    //pl.z = pl.z * 2.0 - 1.0;
    float bl = 0.0;
    float radius = 0.5;
    vec2 distCoord = TexCoords;
    int size = 32;
    float bias = 0.0;
    for (int i = 0; i < size; i++) {
    	distCoord += i / size * size;
    	vec3 random = randomVec(distCoord);
	vec3 se = ep + radius * random;//reflect(random, pl);
	vec2 ss = (se.xy / se.z) * vec2(.75, 1.0);
	vec2 sn = ss * .5 + vec2(.5);
	sn = se.xy / se.z;
	float sz = (2.0 * near) / (far + near - texture2D(gDepth, sn).x * (far - near));
	float zd = 50.0 * max(se.z - sz, 0.0);
    	//float rangeCheck = smoothstep(0.0, 1.0, radius / abs(zbu - sz));
    	//bl += (sz >= se.z + bias ? 1.0 : 0.0) * rangeCheck;
	bl += 1.0 / (1.0 + zd * zd);
	//outColour = vec3(sz);
    }
    return bl / 32.0;
}

float ssao1() {
    vec2 distCoord = TexCoords;
    vec3 cp = ((uView) * texture(gPosition, distCoord)).rgb;
    vec3 cn = texture(gNormal, distCoord).rgb;
    vec3 noise = vec3(mod(rand(TexCoords - 0.1), 2.0) - 1.0, mod(rand(TexCoords.yx - 0.1), 2.0) - 1.0, 0.0);
    vec3 tangent   = normalize(noise - cn * dot(noise, cn));
    vec3 bitangent = cross(cn, tangent);
    mat3 TBN       = mat3(tangent, bitangent, cn);
    float occlusion = 0.0;
    float radius = 0.5;
    float bias = 0.02;
    int kernelSize = 64;
    for (int i = 0; i < kernelSize; ++i) {
	// get sample position
    	distCoord += i / kernelSize;
    	vec3 samp = randomVec(distCoord);
    	float scale = float(i) / float(kernelSize);
    	scale = mix(0.1f, 1.0f, scale * scale);
    	samp *= scale;
    	samp = TBN * samp;
    	samp = cp + samp * radius;
    
    	vec4 offset = vec4(samp, 1.0);
   	offset      = uProjection * offset;    // from world to clip-space
    	offset.xyz /= offset.w;               // perspective divide
    	offset.xyz  = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0  
    	float sampleDepth = ((uView) * texture(gPosition, offset.xy)).z;
    	float rangeCheck = smoothstep(0.0, 1.0, radius / abs(cp.z - sampleDepth));
    	occlusion   += (sampleDepth >= samp.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    return occlusion;
}

float ssao() {
    float count = 16.0;
    float bias = 0.1;
    
    vec4 norz = texture2D(gNormal, TexCoords);
    float depth = texture2D(gDepth, TexCoords).x;
    float radius = 0.1;
    float scale = radius / depth;
    
    float ao = 0.0;
    for(int i = 0; i < count; i++) {
	vec2 randUv = randomFromUv(TexCoords + float(i) / float(count + 1));
	//float w = (1.0 / width) / clamp(depth, aoclamp, 1.0) + (noise.x * (1.0 - noise.x));
	//float h = (1.0 / height) / clamp(depth, aoclamp, 1.0) + (noise.y * (1.0 - noise.y));
    	//vec2 randUv = TexCoords + float(i) / float(count + 1);
    	//vec3 randNor = texture2D(iChannel1, randUv).xyz * 2.0 - 1.0;
    	vec3 randNor = vec3(mod(rand(randUv), 2.0) - 1.0, mod(rand(randUv.yx), 2.0) - 1.0, mod(rand(randUv + 1.0), 2.0) - 1.0);
    	if (dot(norz.xyz, randNor) < 0.0)
    	    randNor *= -1.0;

    	vec2 off = randNor.xy * scale;
    	vec4 sampleNorz = texture2D(gNormal, TexCoords + off);
    	depth = texture2D(gDepth, TexCoords + off).x;
    	float depthDelta = depth - depth;
    
    	vec3 sampleDir = vec3(randNor.xy * radius, depthDelta);
    	//outColour = vec3(depthDelta);
    	float occ = max(0.0, dot(normalize(norz.xyz), normalize(sampleDir)) - bias) / (length(sampleDir) + 1.0);
    	ao += 1.0 - occ;
    }
    ao /= float(count);
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
    //outColour = abs(texture(gNormal, TexCoords).xyz) * ssaoAlone();
    outColour = vec3(ssaoAlone());
    //ivec2 plop = ivec2(TexCoords.x * 1920, TexCoords.y * 1080);
    ivec2 plop = ivec2(distCoord.x * 1920, distCoord.y * 1080);
    outColour = imageLoad(uFractalTexture, plop).xyz / 255.0f + cn + ca + c;
    imageStore(uFractalTexture, plop, uvec4(0.0));

//    float r = step(0.99f, simplexNoise2(TexCoords * 30.0));
//    r = rand(TexCoords.xy * 100.0f);
//
//
//    float dx = 15*(0.005);
//    float dy = 15*(0.005);
//    vec2 coord = vec2(dx*floor(TexCoords.x/dx), dy*floor(TexCoords.y/dy));
//    outColour = texture2D(gPosition, coord).rgb;
}
