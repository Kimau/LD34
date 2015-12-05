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
	return o;
}

////////////////////////////////////////////////////////////////////////
// pixelMain
PSY_SAMPLER_2D( DiffuseTex );

PS_OUTPUT pixelMain( VS_OUTPUT i )
{
	PS_OUTPUT o = (PS_OUTPUT)0;
	float4 TextColour = PSY_SAMPLE_2D( DiffuseTex, i.TexCoord0_.xy );
	float TextFactor = smoothstep( TextSettings_.x, TextSettings_.y, TextColour.a );

	o.Colour_ = float4( TextColour_.xyz, TextFactor * TextColour_.w ) * i.Colour_;

#if 0
	// Add border.
	if( TextFactor < BorderSettings_.x )
	{
		o.Colour_ = float4( BorderColour_.xyz, TextFactor * BorderColour_.w ) * i.Colour_;
	}
#endif

	return o;
}
