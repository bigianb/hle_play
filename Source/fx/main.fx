
//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

float4x4 g_WorldViewProj;  // World * View * Projection transformation
texture g_MeshTexture;              // Color texture for mesh

bool g_useTexture;

//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
sampler MeshTextureSampler = 
sampler_state
{
    Texture = <g_MeshTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

//--------------------------------------------------------------------------------------
// Vertex shader input structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Position   : POSITION;   // vertex position 
	float4 Diffuse    : COLOR0;     // vertex diffuse color (note that COLOR0 is clamped from 0..1)
	float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
};


//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 
    float4 Diffuse    : COLOR0;     // vertex diffuse color (note that COLOR0 is clamped from 0..1)
    float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
};


//--------------------------------------------------------------------------------------
// This shader computes standard transform and lighting
//--------------------------------------------------------------------------------------
VS_OUTPUT vertexShader( VS_INPUT input )
{
    VS_OUTPUT Output;

	Output.Position = mul(input.Position, g_WorldViewProj);
	Output.Diffuse = input.Diffuse;
    Output.TextureUV = input.TextureUV; 
    
    return Output;    
}


//--------------------------------------------------------------------------------------
// Pixel shader output structure
//--------------------------------------------------------------------------------------
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};


PS_OUTPUT pixelShader( VS_OUTPUT In )
{ 
    PS_OUTPUT Output;

	if (g_useTexture) {
		// Modulate
		float4 Cs = tex2D(MeshTextureSampler, In.TextureUV);

		// when modulated, 0x80 in the fragment colour means no change. 0x80 here is 128/255
		Cs.rgb = Cs.rgb * In.Diffuse.rgb;
		Cs = Cs * 255.0 / 128.0;
		Cs = saturate(Cs);
		Output.RGBColor = Cs;
	} else {
		Output.RGBColor = In.Diffuse;
	}

    return Output;
}


technique technique0
{
	pass p0
	{
		VertexShader = compile vs_3_0 vertexShader();
		PixelShader = compile ps_3_0 pixelShader();
	}
}
