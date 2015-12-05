#include <Psybrus.glsl>

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
VS_OUT( vec4, VsTexCoord0 );

void vertexMain()
{
 	vec4 WorldPosition;
	PSY_MAKE_WORLD_SPACE_VERTEX( WorldPosition, InPosition_ );
	PSY_MAKE_CLIP_SPACE_VERTEX( gl_Position, WorldPosition );
	VsTexCoord0 = InTexCoord_;
	VsColour0 = InColour_;
}

#endif

//////////////////////////////////////////////////////////////////////////
// Pixel shader
#if PIXEL_SHADER

PS_IN( vec4, VsColour0 );
PS_IN( vec4, VsTexCoord0 );

#if PSY_OUTPUT_CODE_TYPE == PSY_CODE_TYPE_GLSL_330
out float4 fragColour;
#endif

#if PSY_OUTPUT_CODE_TYPE == PSY_CODE_TYPE_GLSL_ES_100
#define fragColour gl_FragData[0]
#endif

//////////////////////////////////////////////////////////////////////////
// pixelMain
PSY_SAMPLER_2D( DiffuseTex );

void pixelMain()
{
	vec4 TextColour = PSY_SAMPLE_2D( DiffuseTex, VsTexCoord0.xy );
	float TextFactor = smoothstep( TextSettings_.x, TextSettings_.y, TextColour.a );
	fragColour = float4( TextColour_.xyz, TextFactor * TextColour_.w ) * VsColour0;
}

#endif
