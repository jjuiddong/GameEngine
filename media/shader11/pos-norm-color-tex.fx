//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 )
{
	Filter = ANISOTROPIC;
	AddressU = CLAMP;
	AddressV = CLAMP;
};


cbuffer cbPerFrame : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
	float3 gEyePosW;
}


cbuffer cbLight : register( b1 )
{
	float4 gLight_Ambient;
	float4 gLight_Diffuse;
	float4 gLight_Specular;
	float3 gLight_Direction;
	float3 gLight_PosW;
}


cbuffer cbMaetrial : register( b2 )
{
	float4 gMtrl_Ambient;
	float4 gMtrl_Diffuse;
	float4 gMtrl_Specular;
	float4 gMtrl_Emissive;
	float gMtrl_Pow;
}




//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : TEXCOORD0;
    float2 Tex : TEXCOORD1;
    float4 Color : COLOR0;
    float3 toEye : TEXCOORD2;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
	, float4 Color : COLOR )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Normal = normalize( mul(Normal, (float3x3)World) );
    output.Tex = Tex;
    output.Color = Color;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT In ) : SV_Target
{
	float3 L = -gLight_Direction;
	float3 H = normalize(L + normalize(In.toEye));
	float3 N = normalize(In.Normal);

	float4 color  = gLight_Ambient * gMtrl_Ambient
			+ gLight_Diffuse * gMtrl_Diffuse * max(0, dot(N,L))
			+ gLight_Specular * gMtrl_Specular * pow( max(0, dot(N,H)), gMtrl_Pow);

	float4 Out = txDiffuse.Sample(samLinear, In.Tex);
	//return float4(Out.x, Out.y, Out.z, 1);
	return Out;
}



//--------------------------------------------------------------------------------------
// Vertex Shader BuildShadowMap
//--------------------------------------------------------------------------------------
VS_OUTPUT VS_BuildShadowMap(float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
	, float4 Color : COLOR)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Normal = normalize(mul(Normal, (float3x3)World));
	output.Tex = Tex;
	output.Color = Color;
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader BuildShadowMap
//--------------------------------------------------------------------------------------
float4 PS_BuildShadowMap(VS_OUTPUT In) : SV_Target
{
	float3 L = -gLight_Direction;
	float3 H = normalize(L + normalize(In.toEye));
	float3 N = normalize(In.Normal);

	float4 color = gLight_Ambient * gMtrl_Ambient
		+ gLight_Diffuse * gMtrl_Diffuse * max(0, dot(N,L))
		+ gLight_Specular * gMtrl_Specular * pow(max(0, dot(N,H)), gMtrl_Pow);

	float4 Out = txDiffuse.Sample(samLinear, In.Tex);
	//return float4(Out.x, Out.y, Out.z, 1);
	//return color;
	return Out;
}


technique11 Unlit
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetHullShader(NULL);
       	SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}


technique11 ShadowMap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}


technique11 BuildShadowMap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_BuildShadowMap()));
		SetGeometryShader(NULL);
        SetHullShader(NULL);
       	SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_BuildShadowMap()));
	}
}

