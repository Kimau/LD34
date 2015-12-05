#include "Psybrus.hlsl"

////////////////////////////////////////////////////////////////////////
// VS_INPUT
#define VS_INPUT		VertexDefault

////////////////////////////////////////////////////////////////////////
// VS_OUTPUT
struct VS_OUTPUT
{
	float4 Position_	: SV_POSITION;
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
	float4 InputColour = PSY_SAMPLE_2D( InputTexture0, i.TexCoord0_.xy );
	o.Colour_ = InputColour;
	return o;
}
