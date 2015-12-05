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
// PATCH_OUTPUT
struct PATCH_OUTPUT
{
    float Edges_[3] 	: SV_TessFactor;
    float Inside_		: SV_InsideTessFactor;
};


////////////////////////////////////////////////////////////////////////
// PS_OUTPUT
struct PS_OUTPUT
{
	float4 Colour_		: SV_TARGET;
};

////////////////////////////////////////////////////////////////////////
// ScnShaderPostProcessBlurBlockData
BEGIN_CBUFFER( GaTestTextureBlockData )
	ENTRY( GaTestTextureBlockData, float4, UVWOffset_ ) 
END_CBUFFER

#if !PSY_USE_CBUFFER

#  define UVWOffset_ GaTestTextureBlockDataVS_XUVWOffset_

#endif

////////////////////////////////////////////////////////////////////////
// vertexMain
VS_OUTPUT vertexMain( VS_INPUT i )
{
	VS_OUTPUT o = (VS_OUTPUT)0;

	float4 WorldPosition;
	float4 Normal;
	PSY_MAKE_WORLD_SPACE_VERTEX( WorldPosition, i.Position_, i );
	PSY_MAKE_CLIP_SPACE_VERTEX( o.Position_, WorldPosition );
	PSY_MAKE_WORLD_SPACE_NORMAL( Normal, i.Normal_, i );
	o.Colour_ = i.Colour_;

#if TEXTURE_TEST_DIMENSION == 6
	o.TexCoord0_ = Normal;
#else
	o.TexCoord0_ = i.TexCoord0_ + UVWOffset_;
#endif

	return o;
}

////////////////////////////////////////////////////////////////////////
// pixelMain
#if TEXTURE_TEST_DIMENSION == 1
PSY_SAMPLER_1D( DiffuseTex );
#elif TEXTURE_TEST_DIMENSION == 2
PSY_SAMPLER_2D( DiffuseTex );
#elif TEXTURE_TEST_DIMENSION == 3
PSY_SAMPLER_3D( DiffuseTex );
#elif TEXTURE_TEST_DIMENSION == 6
PSY_SAMPLER_CUBE( DiffuseTex );
#endif

PS_OUTPUT pixelMain( VS_OUTPUT i )
{
	PS_OUTPUT o = (PS_OUTPUT)0;
	float4 Colour = float4( 1.0, 0.0, 1.0, 1.0 );
#if TEXTURE_TEST_DIMENSION == 1
	Colour = PSY_SAMPLE_1D( DiffuseTex, i.TexCoord0_.x );
#elif TEXTURE_TEST_DIMENSION == 2
	Colour = PSY_SAMPLE_2D( DiffuseTex, i.TexCoord0_.xy );
#elif TEXTURE_TEST_DIMENSION == 3
	Colour = PSY_SAMPLE_3D( DiffuseTex, i.TexCoord0_.xyz );
#elif TEXTURE_TEST_DIMENSION == 6
	Colour = PSY_SAMPLE_CUBE( DiffuseTex, i.TexCoord0_.xyz );
#endif
	o.Colour_ = Colour * i.Colour_;
	return o;
}
