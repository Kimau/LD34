#include <Psybrus.glsl>

////////////////////////////////////////////////////////////////////////
// GaTestTextureBlockData
BEGIN_CBUFFER( GaTestTextureBlockData )
	ENTRY( GaTestTextureBlockData, vec4, UVWOffset_ ) 
END_CBUFFER

#if !PSY_USE_CBUFFER

#  define UVWOffset_ GaTestTextureBlockDataVS_XUVWOffset_

#endif

//////////////////////////////////////////////////////////////////////////
// Vertex shader
#if VERTEX_SHADER

VS_IN( vec4, InPosition_, POSITION );
VS_IN( vec4, InNormal_, NORMAL );
VS_IN( vec4, InTexCoord_, TEXCOORD0 );
VS_IN( vec4, InColour_, COLOUR0 );

#if defined( PERM_MESH_SKINNED_3D )

VS_IN( vec4, InBlendWeights_, BLENDWEIGHTS );
VS_IN( vec4, InBlendIndices_, BLENDINDICES );

#elif defined( PERM_MESH_PARTICLE_3D )

VS_IN( vec4, InVertexOffset_, TANGENT );

#endif

VS_OUT( vec4, VsColour0 );
VS_OUT( vec4, VsNormal );
VS_OUT( vec4, VsTexCoord0 );

void vertexMain()
{
 	vec4 WorldPosition;
	PSY_MAKE_WORLD_SPACE_VERTEX( WorldPosition, InPosition_ );
	PSY_MAKE_CLIP_SPACE_VERTEX( gl_Position, WorldPosition );
	PSY_MAKE_WORLD_SPACE_NORMAL( VsNormal, InNormal_ );
	VsColour0 = InColour_;

#if TEXTURE_TEST_DIMENSION == 6 && PSY_OUTPUT_CODE_TYPE != PSY_CODE_TYPE_GLSL_ES_100
	VsTexCoord0 = VsNormal;
#else
	VsTexCoord0 = InTexCoord_ + UVWOffset_;
#endif
}

#endif

//////////////////////////////////////////////////////////////////////////
// Pixel shader
#if PIXEL_SHADER

PS_IN( vec4, VsColour0 );
PS_IN( vec4, VsNormal );
PS_IN( vec4, VsTexCoord0 );

#if PSY_OUTPUT_CODE_TYPE == PSY_CODE_TYPE_GLSL_330
out float4 fragColor;
#endif

#if PSY_OUTPUT_CODE_TYPE == PSY_CODE_TYPE_GLSL_ES_100
#define fragColor gl_FragData[0]
#endif

//////////////////////////////////////////////////////////////////////////
// pixelMain
#if TEXTURE_TEST_DIMENSION == 1 && PSY_OUTPUT_CODE_TYPE != PSY_CODE_TYPE_GLSL_ES_100
PSY_SAMPLER_1D( DiffuseTex );
#elif TEXTURE_TEST_DIMENSION == 2
PSY_SAMPLER_2D( DiffuseTex );
#elif TEXTURE_TEST_DIMENSION == 3 && PSY_OUTPUT_CODE_TYPE != PSY_CODE_TYPE_GLSL_ES_100
PSY_SAMPLER_3D( DiffuseTex );
#elif TEXTURE_TEST_DIMENSION == 6 && PSY_OUTPUT_CODE_TYPE != PSY_CODE_TYPE_GLSL_ES_100
PSY_SAMPLER_CUBE( DiffuseTex );
#endif

void pixelMain()
{
	vec4 Colour = vec4( 1.0, 0.0, 1.0, 1.0 );
#if TEXTURE_TEST_DIMENSION == 1 && PSY_OUTPUT_CODE_TYPE != PSY_CODE_TYPE_GLSL_ES_100
	Colour = PSY_SAMPLE_1D( DiffuseTex, VsTexCoord0.x );
#elif TEXTURE_TEST_DIMENSION == 2
	Colour = PSY_SAMPLE_2D( DiffuseTex, VsTexCoord0.xy );
#elif TEXTURE_TEST_DIMENSION == 3 && PSY_OUTPUT_CODE_TYPE != PSY_CODE_TYPE_GLSL_ES_100
	Colour = PSY_SAMPLE_3D( DiffuseTex, VsTexCoord0.xyz );
#elif TEXTURE_TEST_DIMENSION == 6 && PSY_OUTPUT_CODE_TYPE != PSY_CODE_TYPE_GLSL_ES_100
	Colour = PSY_SAMPLE_CUBE( DiffuseTex, VsTexCoord0.xyz );
#endif
	fragColor = Colour * VsColour0;
}

#endif
