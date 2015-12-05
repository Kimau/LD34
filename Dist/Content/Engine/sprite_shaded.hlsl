#include "Psybrus.hlsl"

////////////////////////////////////////////////////////////////////////
// VS_INPUT
#define VS_INPUT		VertexDefault

////////////////////////////////////////////////////////////////////////
// VS_OUTPUT
struct VS_OUTPUT
{
	float4 Position_	: SV_POSITION;
	float4 Colour_		: COLOR0;
	float4 TexCoord0_	: TEXCOORD0;
	float4 RawPosition_	: TEXCOORD1;
};

////////////////////////////////////////////////////////////////////////
// PS_OUTPUT
struct PS_OUTPUT
{
	float4 Colour_		: SV_TARGET;
};

////////////////////////////////////////////////////////////////////////
// vertexMain
VS_OUTPUT vertexMain( VS_INPUT i )
{
	VS_OUTPUT o = (VS_OUTPUT)0;

	float4 WorldPosition;
	PSY_MAKE_WORLD_SPACE_VERTEX( WorldPosition, i.Position_, i );
	PSY_MAKE_CLIP_SPACE_VERTEX( o.Position_, WorldPosition );
	o.Colour_ = i.Colour_;
	o.TexCoord0_ = i.TexCoord0_;
	o.RawPosition_ = i.Position_;
	return o;
}

////////////////////////////////////////////////////////////////////////
// pixelMain
PSY_SAMPLER_2D( DiffuseTex );

PS_OUTPUT pixelMain( VS_OUTPUT i )
{
	PS_OUTPUT o = (PS_OUTPUT)0;

	// Calculate normal.
	const float2 OffsetX = float2( 1.0 / 128.0, 0.0 );
	const float2 OffsetY = float2( 0.0, 1.0 / 128.0 );
	float4 ColourL = PSY_SAMPLE_2D( DiffuseTex, i.TexCoord0_.xy - OffsetX );
	float4 ColourR = PSY_SAMPLE_2D( DiffuseTex, i.TexCoord0_.xy + OffsetX );
	float4 ColourT = PSY_SAMPLE_2D( DiffuseTex, i.TexCoord0_.xy - OffsetY );
	float4 ColourB = PSY_SAMPLE_2D( DiffuseTex, i.TexCoord0_.xy + OffsetY );
	float4 Colour = PSY_SAMPLE_2D( DiffuseTex, i.TexCoord0_.xy );

	float3 Axis = float3( 
		( Colour.a - ColourR.a ) - ( Colour.a - ColourT.a ),
		( Colour.a - ColourB.a ) - ( Colour.a - ColourL.a ),
		0.25 );
	float3 Normal = normalize(
		Axis.x * float3( 1.0, 0.0, 0.0 ) +
		Axis.y * float3( 0.0, 1.0, 0.0 ) +
		Axis.z * float3( 0.0, 0.0, 1.0 ) );

	// Recalculate alpha.
	if( Colour.a < 0.5 )
	{
		Colour.a = 0.0;
	}
	else
	{
		Colour.a = 1.0;
	}

	// Perform lighting.
#if 1
	float4 TotalLight = float4( 0.0, 0.0, 0.0, 0.0 );

	PSY_UNROLL
	for( int Idx = 0; Idx < 1; ++Idx )
	{
		float2 LightVec = LightPosition_[ Idx ].xy - i.RawPosition_.xy;
		float3 LightDir = normalize( float3( LightVec, -0.25 ) );
		float Distance = length( LightVec );
		float InvAttenuation = 
			( LightAttn_[ Idx ].x ) + 
			( Distance * LightAttn_[ Idx ].y ) + 
			( Distance * Distance * LightAttn_[ Idx ].z );
		if( InvAttenuation > 0.0 )
		{
			float Diffuse = ( 1.0 / InvAttenuation ) * saturate( -dot( Normal, LightDir ) );
			TotalLight += 
				LightAmbientColour_[ Idx ] + 
				LightDiffuseColour_[ Idx ] * Diffuse;
		}
	}
#endif

#if 0
	o.Colour_ = float4( ( Normal + 1.0 ) * 0.5, Colour.a );

#else

	// Final colour.
	o.Colour_ = Colour * float4( TotalLight.xyz, 1.0f ) * i.Colour_;
#endif
	return o;
}
