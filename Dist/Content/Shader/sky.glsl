#include <Psybrus.glsl>

////////////////////////////////////////////////////////////////////////
// GaTestCloudBlockData
/*
BEGIN_CBUFFER( GaTestCloudBlockData )
  ENTRY( GaTestCloudBlockData, float, CloudTimer_ )
  ENTRY( GaTestCloudBlockData, vec4, CloudScale_ )
END_CBUFFER

#if !PSY_USE_CBUFFER
#  define CloudTimer_ GaTestCloudBlockDataVS_XCloudTimer_
#  define CloudScale_ GaTestCloudBlockDataVS_XCloudScale_
#  define CloudThreshold_ GaTestCloudBlockDataVS_XCloudThreshold_
#endif*/
////////////////////////////////////////////////////////////////////////

const float pi = 3.1415927;
const float deg = pi / 180.0;
const float CloudTimer_ = 1.0;

// See derivation of noise functions by Morgan McGuire at
// https://www.shadertoy.com/view/4dS3Wd
const int NUM_OCTAVES = 5;
float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) {
  return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) *
               (0.1 + abs(sin(p.y * 13.0 + p.x))));
}
// 1 octave value noise
float noise(float x) {
  float i = floor(x);
  float f = fract(x);
  float u = f * f * (3.0 - 2.0 * f);
  return mix(hash(i), hash(i + 1.0), u);
}
float noise(vec2 x) {
  vec2 i = floor(x);
  vec2 f = fract(x);
  float a = hash(i);
  float b = hash(i + vec2(1.0, 0.0));
  float c = hash(i + vec2(0.0, 1.0));
  float d = hash(i + vec2(1.0, 1.0));
  vec2 u = f * f * (3.0 - 2.0 * f);
  return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}
float noise(vec3 x) {
  const vec3 step = vec3(110, 241, 171);
  vec3 i = floor(x);
  vec3 f = fract(x);
  float n = dot(i, step);
  vec3 u = f * f * (3.0 - 2.0 * f);
  return mix(mix(mix(hash(n + dot(step, vec3(0, 0, 0))),
                     hash(n + dot(step, vec3(1, 0, 0))), u.x),
                 mix(hash(n + dot(step, vec3(0, 1, 0))),
                     hash(n + dot(step, vec3(1, 1, 0))), u.x),
                 u.y),
             mix(mix(hash(n + dot(step, vec3(0, 0, 1))),
                     hash(n + dot(step, vec3(1, 0, 1))), u.x),
                 mix(hash(n + dot(step, vec3(0, 1, 1))),
                     hash(n + dot(step, vec3(1, 1, 1))), u.x),
                 u.y),
             u.z);
}
// Multi-octave value noise
float NOISE(float x) {
  float v = 0.0;
  float a = 0.5;
  float shift = float(100);
  for (int i = 0; i < NUM_OCTAVES; ++i) {
    v += a * noise(x);
    x = x * 2.0 + shift;
    a *= 0.5;
  }
  return v;
}
float NOISE(vec2 x) {
  float v = 0.0;
  float a = 0.5;
  vec2 shift = vec2(100);
  mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
  for (int i = 0; i < NUM_OCTAVES; ++i) {
    v += a * noise(x);
    x = rot * x * 2.0 + shift;
    a *= 0.5;
  }
  return v;
}
// Fast hash2 from https://www.shadertoy.com/view/lsfGWH
float hash2(vec2 co) {
  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

mat3 rotation(float yaw, float pitch) {
  return mat3(cos(yaw), 0, -sin(yaw), 0, 1, 0, sin(yaw), 0, cos(yaw)) *
         mat3(1, 0, 0, 0, cos(pitch), sin(pitch), 0, -sin(pitch), cos(pitch));
}

///////////////////////////////////////////////////////////////////////
// Only globals needed for the actual spheremap
const float screenscale = 1.0 / 800.0f;

float fbm(vec3 x) {
  float r = 0.0, s = 1.0, w = 1.0;
  for (int i = 0; i < 5; i++) {
    s *= 2.0;
    w *= 0.5;
    r += w * noise(s * x);
  }
  return r;
}

float cloud(vec3 dir, vec3 scale, float density, float sharpness, float speed) {
  float b = fbm(scale * (dir + vec3(speed, speed, 0) * CloudTimer_)) -
            (1.0 - density);
  b = clamp(b, 0., 1.);
  return pow(b, 1.0 - sharpness);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Spheremap visualization code from https://www.shadertoy.com/view/4sSXzG
vec4 render(vec3 pos, vec3 dir) {
  vec3 color = vec3(abs(dir.xy), 0.5);

  float sun = smoothstep(0.98, 0.99, dot(dir, normalize(vec3(1.0, 0.4, 0.0))));
  color.rgb += sun * vec3(1.0, 0.3, 0.6);

  float cl = cloud(dir, vec3(5.0, 5.0, 5.0), 0.7, 0.01, 0.03);
  color.rgb = mix(color.rgb, vec3(0.3, 0.0, 0.3), cl);

  float ground = 1.0 - smoothstep(0.0, 0.01, dir.y);
  color.rgb = mix(color.rgb, vec3(0.0, 0.0, 0.7), ground);

  float distToFloor = (pos.y / dir.y);
  vec2 floorpos = pos.xz - dir.xz * distToFloor;
  distToFloor = 1.0 - min(1.0, abs(distToFloor * 0.004));

  float latLongLine =
      distToFloor *
      (1.0 -
       smoothstep(0.03, 0.05, abs(fract(floorpos.x * 0.1) - 0.5)) *
           smoothstep(0.03, 0.05, abs(fract(floorpos.y * 0.1) - 0.5)));

  color.rgb = mix(color.rgb, vec3(0.0, 0.0, 0.0), latLongLine * ground);

  vec4 fragColor;
  fragColor = vec4(color, 1.0);
  return fragColor;
}

//////////////////////////////////////////////////////////////////////////
// Vertex shader
#if VERTEX_SHADER

VS_IN(vec4, InPosition_, POSITION);
VS_IN(vec4, InNormal_, NORMAL);
VS_IN(vec4, InTexCoord_, TEXCOORD0);
VS_IN(vec4, InColour_, COLOUR0);

VS_OUT(vec3, VsRayPos);
VS_OUT(vec3, VsRayDir);

void vertexMain() {
  gl_Position = InPosition_;

  vec2 q = InTexCoord_.xy;
  vec2 p = -1.0 + 2.0 * q;

  // ray near + far
  vec4 rn = mul(InverseProjectionTransform_, vec4(p, 0.0, 1.0));
  vec4 rf = mul(InverseProjectionTransform_, vec4(p, 1.0, 1.0));

  rn *= 0.0;
  rf *= 10000.0;

  rn = mul(InverseViewTransform_, vec4(rn.xyz, 1.0));
  rf = mul(InverseViewTransform_, vec4(rf.xyz, 1.0));

  VsRayPos = rn.xyz;
  VsRayDir = normalize(rf.xyz - rn.xyz);
}

#endif

//////////////////////////////////////////////////////////////////////////
// Pixel shader
#if PIXEL_SHADER

PS_IN(vec3, VsRayPos);
PS_IN(vec3, VsRayDir);

#if PSY_OUTPUT_CODE_TYPE == PSY_CODE_TYPE_GLSL_330
out float4 fragColor;
#endif

#if PSY_OUTPUT_CODE_TYPE == PSY_CODE_TYPE_GLSL_ES_100
#define fragColor gl_FragData[0]
#endif

void pixelMain() { fragColor = render(VsRayPos, normalize(VsRayDir)); }

#endif
