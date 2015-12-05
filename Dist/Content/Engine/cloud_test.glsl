#include <Psybrus.glsl>

////////////////////////////////////////////////////////////////////////
// GaTestCloudBlockData
BEGIN_CBUFFER( GaTestCloudBlockData )
	ENTRY( GaTestCloudBlockData, vec4, CloudTimer_ ) 
	ENTRY( GaTestCloudBlockData, vec4, CloudScale_ ) 
	ENTRY( GaTestCloudBlockData, float, CloudThreshold_ )
	ENTRY( GaTestCloudBlockData, float, CloudPadding0_ )
	ENTRY( GaTestCloudBlockData, float, CloudPadding1_ )
	ENTRY( GaTestCloudBlockData, float, CloudPadding2_ )
END_CBUFFER

#if !PSY_USE_CBUFFER

#  define CloudTimer_ GaTestCloudBlockDataVS_XCloudTimer_
#  define CloudScale_ GaTestCloudBlockDataVS_XCloudScale_
#  define CloudThreshold_ GaTestCloudBlockDataVS_XCloudThreshold_

#endif

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
// map
float map( vec3 Position )
{
	return sdBox( Position, CloudScale_.xyz );
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
// sampleCloud
#if PSY_OUTPUT_CODE_TYPE != PSY_CODE_TYPE_GLSL_ES_100
PSY_SAMPLER_3D( CloudTex );
#endif

float sampleCloud( vec3 Position )
{
	vec3 SamplePosition = ( Position.xzy / ( CloudScale_.xyz * 2.0 ) ) + 0.5;
#if PSY_OUTPUT_CODE_TYPE != PSY_CODE_TYPE_GLSL_ES_100
	vec4 CloudSample = PSY_SAMPLE_3D( CloudTex, SamplePosition + vec3( CloudTimer_.w, 0.0, 0.0 ) );
#else
	vec4 CloudSample = vec4( 0.0, 0.0, 0.0, 0.0 );
#endif
	return max( 0.0, CloudSample.x );
}

//////////////////////////////////////////////////////////////////////////
// calcNormal
vec3 calcNormal( vec3 Position )
{
	vec3 Offset = vec3( 0.01, 0.0, 0.0 );
	vec3 Normal = vec3(
		sampleCloud( Position - Offset.xyz ) - sampleCloud( Position + Offset.xyz ),
		sampleCloud( Position - Offset.zxy ) - sampleCloud( Position + Offset.zxy ),
		sampleCloud( Position - Offset.yzx ) - sampleCloud( Position + Offset.yzx ) );
	return normalize( Normal );
}

//////////////////////////////////////////////////////////////////////////
// calcOcclusion
float calcOcclusion( vec3 Position )
{
	vec3 Offset = vec3( 0.01, 0.0, 0.0 );
	vec3 Offsets[ 6 ];
	Offsets[ 0 ] = -Offset.xyz;
	Offsets[ 1 ] = Offset.xyz;
	Offsets[ 2 ] = -Offset.zxy;
	Offsets[ 3 ] = Offset.zxy;
	Offsets[ 4 ] = -Offset.yzx;
	Offsets[ 5 ] = Offset.yzx;

	float Occlusion = 0.0;
	for( int Idx = 0; Idx < 6; ++Idx )
	{
		if( sampleCloud( Position + Offsets[ Idx ] ) > CloudThreshold_ )
		{
			Occlusion += 1.0 / 6.0;
		}
	}

	return Occlusion;
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

		// Step fast until we hit a cloud, then refine by halving the step size.
		float Precision = 0.01;
		float StepSize = 0.1;
		float Tmin = 1.0;
		float Tmax = 1000.0;
		int MaxSteps = 256;
		int RefinementSteps = 4;
		int NoofRefinements = 0;
		float T = Tmin;
		float Dist = -1.0;
		for( int Idx = 0; Idx < MaxSteps; ++Idx )
		{
			vec3 SamplePosition = Position + RayDir * T;
			Dist = map( SamplePosition );

			if( Dist > Precision || T > Tmax )
			{
				break;
			}
#if 0
			if( Dist < -0.5 )
			{
				Colour.xyz = vec3( 1.0, 1.0, 1.0 ) * ( ( 0.5 * sampleCloud( SamplePosition ) + 0.5 ) / CloudThreshold_ );
				Colour.w = 1.0;
				break;
			}
			T += StepSize;
#else
			if( sampleCloud( SamplePosition ) > CloudThreshold_ )
			{
				if( NoofRefinements < RefinementSteps )
				{
					T -= StepSize;
					StepSize *= 0.5;
					NoofRefinements++;
				}
				else
				{
					Colour.xyz = vec3( 1.0, 1.0, 1.0 );// * ( 1.0 - calcOcclusion( SamplePosition ) );
					Colour.w = 1.0;

					// TODO: Perform more work for this pixel for better quality cloud.

					break;
				}
			}
			else
			{
				T += StepSize;
			}
#endif
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
