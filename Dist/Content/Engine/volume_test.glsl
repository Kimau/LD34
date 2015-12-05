#include <Psybrus.glsl>

//////////////////////////////////////////////////////////////////////////
// Vertex shader
#if VERTEX_SHADER

VS_IN( vec4, InPosition_, POSITION );
VS_IN( vec4, InNormal_, NORMAL );
VS_IN( vec4, InTexCoord_, TEXCOORD0 );
VS_IN( vec4, InColour_, COLOUR0 );

VS_OUT( vec3, VsRayPos );
VS_OUT( vec3, VsRayDir );

void vertexMain()
{
	gl_Position = InPosition_;

	vec2 q = InTexCoord_.xy;
	vec2 p = -1.0+2.0*q;
		 
	// ray near + far
	vec4 rn = mul( InverseProjectionTransform_, vec4( p, 0.0, 1.0 ) );
	vec4 rf = mul( InverseProjectionTransform_, vec4( p, 1.0, 1.0 ) );

	rn *= 0.0;
	rf *= 10000.0;

	rn = mul( InverseViewTransform_, vec4( rn.xyz, 1.0 ) );
	rf = mul( InverseViewTransform_, vec4( rf.xyz, 1.0 ) );

	VsRayPos = rn.xyz;
	VsRayDir = normalize( rf.xyz - rn.xyz );
}

#endif

//////////////////////////////////////////////////////////////////////////
// Pixel shader
#if PIXEL_SHADER

PS_IN( vec3, VsRayPos );
PS_IN( vec3, VsRayDir );

#if PSY_OUTPUT_CODE_TYPE == PSY_CODE_TYPE_GLSL_330
out float4 fragColor;
#endif

#if PSY_OUTPUT_CODE_TYPE == PSY_CODE_TYPE_GLSL_ES_100
#define fragColor gl_FragData[0]
#endif

//////////////////////////////////////////////////////////////////////////
// sdSphere
float sdSphere( vec3 Position, float Radius )
{
	return length( Position ) - Radius;
}

//////////////////////////////////////////////////////////////////////////
// sdBox
float sdBox( vec3 Position, vec3 Bounds )
{
  vec3 Distance = abs( Position ) - Bounds;
  return min( max( Distance.x, max( Distance.y, Distance.z ) ), 0.0 ) + length( max( Distance, 0.0 ) );
}

//////////////////////////////////////////////////////////////////////////
// sdCloud
float cosNoise( float3 Position )
{
	float Ret = 0.0;
	float Mul = 1.0;
	float Freq = 0.2;
	for( int Idx = 0; Idx < 4; ++Idx )
	{
		Ret += ( cos( Position.x * Freq ) * Mul + sin( Position.y * Freq ) * Mul + cos( ( Position.z + 0.2 ) * Freq ) * Mul ) / 3.0;
		Mul *= 0.5;
		Freq *= 2.0;
	}
	return Ret;
}

float sdCloud( vec3 Position )
{
	return cosNoise( Position ); 
}

//////////////////////////////////////////////////////////////////////////
// map
float map( vec3 Position )
{
	return sdBox( Position, vec3( 8.0, 1.0, 8.0 ) );
}

//////////////////////////////////////////////////////////////////////////
// rayMarch
vec2 rayMarch( vec3 RayPos, vec3 RayDir )
{
	float Precision = 0.001;
	float Tmin = 1.0;
	float Tmax = 1000.0;
	int MaxSteps = 128;
	float T = Tmin;
	float Dist = -1.0;
	for( int Idx = 0; Idx < MaxSteps; ++Idx )
	{
		vec3 SamplePosition = RayPos + RayDir * T;
		Dist = map( SamplePosition );
		if( Dist < Precision || T > Tmax )
		{
			break;
		}
		T += Dist;
	}
	if( T > Tmax )
	{
		Dist = -1.0;
	}
	return vec2( T, Dist );
}

//////////////////////////////////////////////////////////////////////////
// render
vec4 render( vec3 RayPos, vec3 RayDir )
{
	vec4 Colour = vec4( 0.0, 0.0, 0.0, 0.0 );

	// Find intersecting point.
	vec2 RayRes = rayMarch( RayPos, RayDir );
	if( RayRes.y >= -0.5 )
	{
		vec3 Position = RayPos + RayDir * RayRes.x;

		vec4 Colour = vec4( 1.0, 1.0, 1.0, 0.0 );
		float Density = 0.1;

		// TODO: Transform position from world to local for 3D texture look ups later?

		// March to opposite side of volume, accumulate alpha.
		float Precision = 0.001;
		float StepSize = 0.1;
		float Tmin = 1.0;
		float Tmax = 1000.0;
		int MaxSteps = 128;
		float T = Tmin;
		float Dist = -1.0;
		for( int Idx = 0; Idx < MaxSteps; ++Idx )
		{
			vec3 SamplePosition = Position + RayDir * T;
			Dist = map( SamplePosition );
			if( Dist > Precision || T > Tmax || Density > 1.0 )
			{
				break;
			}
			T += StepSize;
			Colour.w += Density * StepSize * max( 0.0, sdCloud( SamplePosition ) );
		}

		//
		return Colour;
	}
	return vec4( 0.0, 0.0, 0.0, 0.0 );
}

//////////////////////////////////////////////////////////////////////////
// pixelMain
void pixelMain()
{
	fragColor = render( VsRayPos, normalize( VsRayDir ) );
}

#endif // PIXEL_SHADER
