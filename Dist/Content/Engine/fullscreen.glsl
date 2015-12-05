#include <Psybrus.glsl>

//////////////////////////////////////////////////////////////////////////
// Vertex shader
#if VERTEX_SHADER

VS_IN( vec4, InPosition_, POSITION );
VS_IN( vec4, InNormal_, NORMAL );
VS_IN( vec4, InTexCoord_, TEXCOORD0 );
VS_IN( vec4, InColour_, COLOUR0 );

VS_OUT( vec4, VsColour0 );
VS_OUT( vec4, VsNormal );
VS_OUT( vec4, VsTexCoord0 );

void vertexMain()
{
	gl_Position = InPosition_;
	VsNormal = InNormal_;
	VsTexCoord0 = InTexCoord_;
	VsColour0 = InColour_;
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
PSY_SAMPLER_2D( ColourTex );
PSY_SAMPLER_2D( DepthTex );

void pixelMain()
{
	vec4 Colour = PSY_SAMPLE_2D( ColourTex, VsTexCoord0.xy );
	float Depth = PSY_SAMPLE_2D( DepthTex, VsTexCoord0.xy ).x;
	fragColor = Colour; //float4( Depth, Depth, Depth, 1.0 ) * i.Colour_;
}

#endif
