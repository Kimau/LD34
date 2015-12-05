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

	float Offset = ( i.TexCoord0_.z * i.TexCoord0_.w ) * 0.5f;
	float3 WorldPosition = i.Position_.xyz;
	float3 ViewNormal = normalize( float3( mul( ViewTransform_, i.Normal_ ).xy, 0.0001 ) );
	float3 CrossZ = cross( ViewNormal, float3( 0.0, 0.0, 1.0 ) );
	float4 ViewPosition = mul( ViewTransform_, float4( WorldPosition, 1.0 ) );
	o.Position_ = mul( ProjectionTransform_, ViewPosition + float4( CrossZ * Offset, 0.0 ) );
	o.Colour_ = i.Colour_;
	o.TexCoord0_ = i.TexCoord0_;
	return o;
}

////////////////////////////////////////////////////////////////////////
// pixelMain
PS_OUTPUT pixelMain( VS_OUTPUT i )
{
	PS_OUTPUT o = (PS_OUTPUT)0;
	o.Colour_ = i.Colour_;
	return o;
}
