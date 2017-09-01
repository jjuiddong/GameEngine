//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#define SHADOW_EPSILON 0.001f

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
Texture2D txShadow : register(t1);

SamplerState samLinear : register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState samShadow : register(s1)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Border;
	AddressV = Border;
	//BorderColor = 0xffffffff;
};



cbuffer cbPerFrame : register( b0 )
{
	matrix gWorld;
	matrix gView;
	matrix gProjection;
	matrix gLightView;
	matrix gLightProj;
	matrix gLightTT;
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
    float3 toEye : TEXCOORD2;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, gWorld );
    output.Pos = mul( output.Pos, gView );
    output.Pos = mul( output.Pos, gProjection );
    output.Normal = normalize( mul(Normal, (float3x3)gWorld) );
    output.Tex = Tex;
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

	float4 Out = color * txDiffuse.Sample( samLinear, In.Tex );
	return Out;
}



//--------------------------------------------------------------------------------------
// Vertex Shader ShadowMap
//--------------------------------------------------------------------------------------
struct VS_SHADOW_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : TEXCOORD0;
	float2 Tex : TEXCOORD1;
	float4 TexShadow : TEXCOORD2;
	float3 toEye : TEXCOORD3;
	float4 Depth : TEXCOORD4;
};

VS_SHADOW_OUTPUT VS_ShadowMap(float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
)
{
	VS_SHADOW_OUTPUT output = (VS_SHADOW_OUTPUT)0;
	output.Pos = mul(Pos, gWorld);
	output.Pos = mul(output.Pos, gView);
	output.Pos = mul(output.Pos, gProjection);
	output.Normal = normalize(mul(Normal, (float3x3)gWorld));
	output.Tex = Tex;

	float4 wPos = mul(Pos, gWorld);
	matrix mLVP = mul(gLightView, gLightProj);
	matrix mVPT = mul(mLVP, gLightTT);
	output.TexShadow = mul(wPos, mVPT);
	output.Depth.xy = mul(wPos, mLVP).zw;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader ShadowMap
//--------------------------------------------------------------------------------------
float4 PS_ShadowMap(VS_SHADOW_OUTPUT In) : SV_Target
{
	float4 vTexCoords[9];
	float fTexelSize = 1.0f / 1024.0f;
	float depth1 = min(In.Depth.x / In.Depth.y, 1.0);

	vTexCoords[0] = In.TexShadow;
	vTexCoords[1] = In.TexShadow + float4(-fTexelSize, 0.0f, 0.0f, 0.0f);
	vTexCoords[2] = In.TexShadow + float4(fTexelSize, 0.0f, 0.0f, 0.0f);
	vTexCoords[3] = In.TexShadow + float4(0.0f, -fTexelSize, 0.0f, 0.0f);
	vTexCoords[6] = In.TexShadow + float4(0.0f, fTexelSize, 0.0f, 0.0f);
	vTexCoords[4] = In.TexShadow + float4(-fTexelSize, -fTexelSize, 0.0f, 0.0f);
	vTexCoords[5] = In.TexShadow + float4(fTexelSize, -fTexelSize, 0.0f, 0.0f);
	vTexCoords[7] = In.TexShadow + float4(-fTexelSize, fTexelSize, 0.0f, 0.0f);
	vTexCoords[8] = In.TexShadow + float4(fTexelSize, fTexelSize, 0.0f, 0.0f);

	float S1 = (depth1 - SHADOW_EPSILON);

	float fShadowTerms[9];
	float fShadowTerm = 0.0f;
	for (int i = 0; i < 9; i++)
	{
		const float2 uv = vTexCoords[i].xy / vTexCoords[i].w;
		const float D1 = txShadow.Sample(samShadow, uv).r;

		fShadowTerms[i] = ((D1 < S1) ) ? 0.1f : 1.f;
		fShadowTerm += fShadowTerms[i];
	}

	fShadowTerm /= 9.0f;

	float3 L = -gLight_Direction;
	float3 H = normalize(L + normalize(In.toEye));
	float3 N = normalize(In.Normal);

	float4 color = gLight_Ambient * gMtrl_Ambient
		+ gLight_Diffuse * gMtrl_Diffuse * 0.2
		+ gLight_Diffuse * gMtrl_Diffuse * max(0, dot(N,L)) * fShadowTerm
		+ gLight_Specular * gMtrl_Specular * pow(max(0, dot(N,H)), gMtrl_Pow);

	float4 Out = color * txDiffuse.Sample(samLinear, In.Tex);
	return Out;
}


//--------------------------------------------------------------------------------------
// Vertex Shader BuildShadowMap
//--------------------------------------------------------------------------------------
struct VS_BUILDSHADOW_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Depth : TEXCOORD0;
};


VS_BUILDSHADOW_OUTPUT VS_BuildShadowMap(float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
)
{
	VS_BUILDSHADOW_OUTPUT output = (VS_BUILDSHADOW_OUTPUT)0;
	output.Pos = mul(Pos, gWorld);
	output.Pos = mul(output.Pos, gView);
	output.Pos = mul(output.Pos, gProjection);
	output.Depth.xy = output.Pos.zw;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader BuildShadowMap
//--------------------------------------------------------------------------------------
float4 PS_BuildShadowMap(
	VS_BUILDSHADOW_OUTPUT In
	) : SV_Target
{
	return In.Depth.x / In.Depth.y;
}


technique11 Unlit
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}


technique11 ShadowMap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_ShadowMap()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_ShadowMap()));
	}
}


technique11 BuildShadowMap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_BuildShadowMap()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_BuildShadowMap()));
	}
}

