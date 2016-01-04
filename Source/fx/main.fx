
//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

float4x4 g_WorldViewProj;  // World * View * Projection transformation
texture g_MeshTexture;              // Color texture for mesh

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

	// Modulate
	float4 Cs = tex2D(MeshTextureSampler, In.TextureUV);
	Cs.rgb *= In.Diffuse.rgb;
	Cs.rgb *= 2.0;			// 0x80 is 1.0
	Cs.a = In.Diffuse.a;

	// function 0x44: Cv = (Cs - Cd) * As >> 7 + Cd

	// Need to get Cd from the frame buffer.
	//Output.RGBColor.rgb = (Cs.rgb - Cd.rgb) * Cs.a;
	//Output.RGBColor.rgb *= 2.0;
	//Output.RGBColor.rgb += Cd.rgb;

	Output.RGBColor = Cs;

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
