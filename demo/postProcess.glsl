#version 430 core

layout (location = 0) uniform float uTime;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDepth;

in vec2 TexCoords;

out vec3 outColour;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 permute(vec3 x0,vec3 p) { 
    vec3 x1 = mod(x0 * p.y, p.x);
    return floor(  mod( (x1 + p.z) *x0, p.x ));
}

float simplexNoise2(vec2 v)
{
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

void main() { 
    vec2 distCoord = TexCoords * 2.0f;
    vec3 cp = texture(gPosition, distCoord).rgb;
    vec3 cn = texture(gNormal, distCoord - vec2(1.0f,0.0f)).rgb;
    vec3 ca = texture(gAlbedoSpec, distCoord - vec2(0.0f,1.0f)).rgb;
    float z = texture(gDepth, distCoord - vec2(1.0f,1.0f)).r;
    float near = 0.1;                                // the near plane
    float far = 10000.0;                               // the far plane
    float c = (2.0 * near) / (far + near - z * (far - near));  // convert to linear values 
    vec3 cd = vec3(c);
    outColour = cn + cp + ca + cd;
    outColour = abs(texture(gNormal, TexCoords).rgb);

//    float r = step(0.99f, simplexNoise2(TexCoords * 30.0));
//    r = rand(TexCoords.xy * 100.0f);
//
//
//    float dx = 15*(0.005);
//    float dy = 15*(0.005);
//    vec2 coord = vec2(dx*floor(TexCoords.x/dx), dy*floor(TexCoords.y/dy));
//    outColour = texture2D(gPosition, coord).rgb;
}
