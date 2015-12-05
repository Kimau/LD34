#include "Psybrus.hlsl"


////////////////////////////////////////////////////////////////////////
// VS_INPUT
#define VS_INPUT		VertexDefault

////////////////////////////////////////////////////////////////////////
// VS_OUTPUT
struct VS_OUTPUT
{
	float4 Position_	: SV_POSITION;
	float4 Normal_		: NORMAL;
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
	PSY_MAKE_WORLD_SPACE_NORMAL( o.Normal_.xyzw, i.Normal_.xyzw, i );
	return o;
}

////////////////////////////////////////////////////////////////////////
// pixelMain
PS_OUTPUT pixelMain( VS_OUTPUT i )
{
	PS_OUTPUT o = (PS_OUTPUT)0;
	o.Colour_.rgb = ( i.Normal_.rgb + float3( 1.0f, 1.0f, 1.0f ) ) * 0.5f;
	o.Colour_.w = 1.0f;
	return o;
}
