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

//#if PSY_OUTPUT_CODE_TYPE == PSY_CODE_TYPE_GLSL_ES_100
#define VsColour0 GsColour0
#define VsNormal GsNormal
#define VsTexCoord0 GsTexCoord0
//#endif

VS_OUT( vec4, VsColour0 );
VS_OUT( vec4, VsNormal );
VS_OUT( vec4, VsTexCoord0 );

void vertexMain()
{
 	vec4 WorldPosition;
	PSY_MAKE_WORLD_SPACE_VERTEX( WorldPosition, InPosition_ );
	PSY_MAKE_CLIP_SPACE_VERTEX( gl_Position, WorldPosition );
	VsNormal = InNormal_;
	VsTexCoord0 = InTexCoord_;
	VsColour0 = InColour_;
}

#endif

//////////////////////////////////////////////////////////////////////////
// Geometry shader
#if GEOMETRY_SHADER

layout(triangles) in;
layout(triangle_strip, max_vertices = 32) out;

in VertexDataIn
{
	vec4 VsColour;
	vec4 VsNormal;
	vec4 VsTexCoord0;
} gs_in[];

out VertexDataOut
{
	vec4 GsColour0;
	vec4 GsNormal;
	vec4 GsTexCoord0;
} gs_out;

void geometryMain()
{
	for( float j = -4; j <= 4; j += 1.0 )
	{
		for( int i = 0; i < 3; ++i )
		{
			float4 WorldPosition = gl_in[i].gl_Position;
			//PSY_MAKE_WORLD_SPACE_VERTEX( WorldPosition, gl_in[i].gl_Position );

			WorldPosition.z += j * 10.0;

			PSY_MAKE_CLIP_SPACE_VERTEX( gl_Position, WorldPosition );

			gs_out.GsNormal = gs_in[i].VsNormal;
			gs_out.GsColour0 = gs_in[i].VsColour;
			gs_out.GsTexCoord0 = gs_in[i].VsTexCoord0;

			EmitVertex();
		}
		EndPrimitive();
	}
}

#endif

//////////////////////////////////////////////////////////////////////////
// Pixel shader
#if PIXEL_SHADER

PS_IN( vec4, GsColour0 );
PS_IN( vec4, GsNormal );
PS_IN( vec4, GsTexCoord0 );

#if PSY_OUTPUT_CODE_TYPE == PSY_CODE_TYPE_GLSL_330
out float4 fragColor;
#endif

#if PSY_OUTPUT_CODE_TYPE == PSY_CODE_TYPE_GLSL_ES_100
#define fragColor gl_FragData[0]
#endif

//////////////////////////////////////////////////////////////////////////
// pixelMain
void pixelMain()
{
	fragColor = GsColour0;
}

#endif
