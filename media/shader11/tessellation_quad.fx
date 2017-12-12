//
// Tessellation Test Shader
//

//--------------------------------------------------------------------------------------
// SamplerState
//--------------------------------------------------------------------------------------
SamplerState samLinear : register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
	BorderColor = float4(1, 1, 1, 1);
};

Texture2D txDiffuse : register(t0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer TessellationBuffer : register(b1)
{
	float4 eyePos;
	float tessellationAmount;
	float3 padding;
};

struct HullInputType
{
	float2 origin   : ORIGIN;
	float2 size     : SIZE;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;

	float2 origin   : ORIGIN;
	float2 size     : SIZE;
};

struct DUMMY
{
	float Dummmy : DUMMY;
};

struct HullOutputType
{
	float2 origin   : ORIGIN;
	float2 size     : SIZE;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};


HullInputType main(
	float4 position : POSITION
	, float2 size : TEXCOORD0
)
{
	HullInputType output;
	output.origin = position.xz;
	output.size = size;
	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Patch Constant Function
////////////////////////////////////////////////////////////////////////////////
ConstantOutputType ColorPatchConstantFunction(InputPatch<HullInputType, 4> inputPatch
	, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	const float2 center = inputPatch[0].origin + inputPatch[0].size * 0.5f;
	const float distance = length(center - eyePos.xz);
	float tessFactor = lerp(1.f, (1 / (0.005*distance)), 1);

	// Set the tessellation factors for the three edges of the triangle.
	output.edges[0] = tessFactor;
	output.edges[1] = tessFactor;
	output.edges[2] = tessFactor;
	output.edges[3] = tessFactor;

	// Set the tessellation factor for tessallating inside the triangle.
	output.inside[0] = tessFactor;
	output.inside[1] = tessFactor;

	output.origin = inputPatch[0].origin;
	output.size = inputPatch[0].size;

	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Hull Shader
////////////////////////////////////////////////////////////////////////////////
[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ColorPatchConstantFunction")]

HullOutputType ColorHullShader(InputPatch<HullInputType, 4> patch
	, uint pointId : SV_OutputControlPointID
	, uint patchId : SV_PrimitiveID)
{
	HullOutputType output = (HullOutputType)0;
	output.origin = patch[pointId].origin;
	output.size = patch[pointId].size;
	return output;
}



////////////////////////////////////////////////////////////////////////////////
// Domain Shader9
////////////////////////////////////////////////////////////////////////////////
[domain("quad")]

PixelInputType ColorDomainShader(ConstantOutputType input
	, float2 uv : SV_DomainLocation
	, const OutputPatch<HullOutputType, 4> patch)
{
	float3 vertexPosition;
	PixelInputType output = (PixelInputType)0;

	float2 v1 = lerp(patch[0].origin, patch[1].origin, uv.x);
	float2 v2 = lerp(patch[2].origin, patch[3].origin, uv.x);
	vertexPosition.xz = lerp(v1, v2, uv.y);
	vertexPosition.y = 0.1f;

	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Send the input color into the pixel shader.
	//output.color = float4(0,0,0,1);
	output.tex = uv;

	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
	float4 texColor = txDiffuse.Sample(samLinear, input.tex);
	//return float4(0, 0, 0, 1); //input.color;
	return texColor;// 
}



technique11 Tech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, main()));
		SetHullShader(CompileShader(hs_5_0, ColorHullShader()));
		SetDomainShader(CompileShader(ds_5_0, ColorDomainShader()));
		SetPixelShader(CompileShader(ps_5_0, ColorPixelShader()));
	}
}
