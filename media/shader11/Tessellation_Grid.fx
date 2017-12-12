//
// Tessellation Test Shader
//

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

//struct VertexInputType
//{
//	float4 position : POSITION;
//	float2 size : TEXCOORD0;
//	//float4 color : COLOR;
//};

struct HullInputType
{
	float2 origin   : ORIGIN;
	float2 size     : SIZE;
	//float4 color : COLOR;
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
	//float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	//float4 color : COLOR;
};


HullInputType main(//VertexInputType input)
	float4 position : POSITION
	, float2 size : TEXCOORD0
)
{
	HullInputType output;
	output.origin = position.xz;
	output.size = size;
	//output.color = input.color;
	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Patch Constant Function
////////////////////////////////////////////////////////////////////////////////
ConstantOutputType ColorPatchConstantFunction(InputPatch<HullInputType, 1> inputPatch
	, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	const float2 center = inputPatch[0].origin + inputPatch[0].size * 0.5f;
	const float distance = length(center - eyePos.xz);
	float tessFactor = lerp(1.f, (1 / (0.005*distance)), 1);
	//float tessFactor = 1;// lerp(1.f, (1 / (0.005*distance)), 1);

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
[outputcontrolpoints(1)]
[patchconstantfunc("ColorPatchConstantFunction")]

DUMMY ColorHullShader(InputPatch<HullInputType, 1> patch
	, uint pointId : SV_OutputControlPointID
	, uint patchId : SV_PrimitiveID)
{
	DUMMY output = (DUMMY)0;
	return output;
}



////////////////////////////////////////////////////////////////////////////////
// Domain Shader9
////////////////////////////////////////////////////////////////////////////////
[domain("quad")]

PixelInputType ColorDomainShader(ConstantOutputType input
	, float2 uv : SV_DomainLocation
	, const OutputPatch<DUMMY, 1> patch)
{
	float3 vertexPosition;
	PixelInputType output = (PixelInputType)0;

	vertexPosition.xz = input.origin + uv * input.size;
	//vertexPosition.xz = float2(0,0) + uv * float2(10,10);
	vertexPosition.y = 0.1f;

	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Send the input color into the pixel shader.
	//output.color = float4(0,0,0,1);

	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
	return float4(0,0,0,1); //input.color;
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
