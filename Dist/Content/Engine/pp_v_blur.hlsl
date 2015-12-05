#include "Psybrus.hlsl"

////////////////////////////////////////////////////////////////////////
// VS_INPUT
struct VS_INPUT
{
	float4 Position_		: POSITION;
	float2 TexCoord0_		: TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////
// VS_OUTPUT
struct VS_OUTPUT
{
	float4 Position_	: SV_POSITION;
	float2 TexCoord0_	: TEXCOORD0;
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
	o.Position_ = i.Position_.xyzw;
	o.TexCoord0_ = i.TexCoord0_;
	return o;
}

////////////////////////////////////////////////////////////////////////
// pixelMain
PSY_SAMPLER_2D( InputTexture0 );

PS_OUTPUT pixelMain( VS_OUTPUT i )
{
	PS_OUTPUT o = (PS_OUTPUT)0;

	const float Offset = 1.0f / InputDimensions_[ 0 ].x;
	const float Radius = Radius_;
	float4 Colour = float4( 0.0, 0.0, 0.0, 0.0 );
	float4 UVOffset = float4( 0.0, -Radius * Offset, 0.0, 0.0 );
	float4 UVIncr = float4( 0.0, Offset, 0.0, 0.0 );

	for( float Idx = -Radius; Idx <= Radius; Idx += 1.0 )
	{
		Colour += PSY_SAMPLE_2D( InputTexture0, i.TexCoord0_.xy + UVOffset.xy );
		UVOffset += UVIncr;
	}
	o.Colour_ = float4( ( Colour / ( Radius * 2.0 + 1.0 ) ).rgb, 1.0 );
	return o;
}
