//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#define SHADOW_EPSILON 0.0001f

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
Texture2D txShadow0 : register(t1);
Texture2D txShadow1 : register(t2);
Texture2D txShadow2 : register(t3);

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
	BorderColor = float4(1,1,1,1);
};



cbuffer cbPerFrame : register( b0 )
{
	matrix gWorld;
	matrix gView;
	matrix gProjection;
	matrix gLightView[3];
	matrix gLightProj[3];
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
	float4 TexShadow0 : TEXCOORD2;
	float4 TexShadow1 : TEXCOORD3;
	float4 TexShadow2 : TEXCOORD4;
	float3 toEye : TEXCOORD5;
	float4 Depth0 : TEXCOORD6;
	float2 Depth1 : TEXCOORD7;
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

	matrix mLVP[3];
	matrix mVPT[3];

	float4 wPos = mul(Pos, gWorld);
	mLVP[0] = mul(gLightView[0], gLightProj[0]);
	mVPT[0] = mul(mLVP[0], gLightTT);
	mLVP[1] = mul(gLightView[1], gLightProj[1]);
	mVPT[1] = mul(mLVP[1], gLightTT);
	mLVP[2] = mul(gLightView[2], gLightProj[2]);
	mVPT[2] = mul(mLVP[2], gLightTT);

	output.TexShadow0 = mul(wPos, mVPT[0]);
	output.TexShadow1 = mul(wPos, mVPT[1]);
	output.TexShadow2 = mul(wPos, mVPT[2]);

	output.Depth0.xy = mul(wPos, mLVP[0]).zw;
	output.Depth0.zw = mul(wPos, mLVP[1]).zw;
	output.Depth1.xy = mul(wPos, mLVP[2]).zw;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader ShadowMap
//--------------------------------------------------------------------------------------
float4 PS_ShadowMap(VS_SHADOW_OUTPUT In) : SV_Target
{
	float4 vTexCoords[3][9];
	float fTexelSize = 1.0f / 1024.0f;
	float depth0 = min(In.Depth0.x / In.Depth0.y, 1.0);
	float depth1 = min(In.Depth0.z / In.Depth0.w, 1.0);
	float depth2 = min(In.Depth1.x / In.Depth1.y, 1.0);

	// Generate the tecture co-ordinates for the specified depth-map size
	// 4 3 5
	// 1 0 2
	// 7 6 8
	vTexCoords[0][0] = In.TexShadow0;
	vTexCoords[0][1] = In.TexShadow0 + float4(-fTexelSize, 0.0f, 0.0f, 0.0f);
	vTexCoords[0][2] = In.TexShadow0 + float4(fTexelSize, 0.0f, 0.0f, 0.0f);
	vTexCoords[0][3] = In.TexShadow0 + float4(0.0f, -fTexelSize, 0.0f, 0.0f);
	vTexCoords[0][6] = In.TexShadow0 + float4(0.0f, fTexelSize, 0.0f, 0.0f);
	vTexCoords[0][4] = In.TexShadow0 + float4(-fTexelSize, -fTexelSize, 0.0f, 0.0f);
	vTexCoords[0][5] = In.TexShadow0 + float4(fTexelSize, -fTexelSize, 0.0f, 0.0f);
	vTexCoords[0][7] = In.TexShadow0 + float4(-fTexelSize, fTexelSize, 0.0f, 0.0f);
	vTexCoords[0][8] = In.TexShadow0 + float4(fTexelSize, fTexelSize, 0.0f, 0.0f);

	vTexCoords[1][0] = In.TexShadow1;
	vTexCoords[1][1] = In.TexShadow1 + float4(-fTexelSize, 0.0f, 0.0f, 0.0f);
	vTexCoords[1][2] = In.TexShadow1 + float4(fTexelSize, 0.0f, 0.0f, 0.0f);
	vTexCoords[1][3] = In.TexShadow1 + float4(0.0f, -fTexelSize, 0.0f, 0.0f);
	vTexCoords[1][6] = In.TexShadow1 + float4(0.0f, fTexelSize, 0.0f, 0.0f);
	vTexCoords[1][4] = In.TexShadow1 + float4(-fTexelSize, -fTexelSize, 0.0f, 0.0f);
	vTexCoords[1][5] = In.TexShadow1 + float4(fTexelSize, -fTexelSize, 0.0f, 0.0f);
	vTexCoords[1][7] = In.TexShadow1 + float4(-fTexelSize, fTexelSize, 0.0f, 0.0f);
	vTexCoords[1][8] = In.TexShadow1 + float4(fTexelSize, fTexelSize, 0.0f, 0.0f);

	vTexCoords[2][0] = In.TexShadow2;
	vTexCoords[2][1] = In.TexShadow2 + float4(-fTexelSize, 0.0f, 0.0f, 0.0f);
	vTexCoords[2][2] = In.TexShadow2 + float4(fTexelSize, 0.0f, 0.0f, 0.0f);
	vTexCoords[2][3] = In.TexShadow2 + float4(0.0f, -fTexelSize, 0.0f, 0.0f);
	vTexCoords[2][6] = In.TexShadow2 + float4(0.0f, fTexelSize, 0.0f, 0.0f);
	vTexCoords[2][4] = In.TexShadow2 + float4(-fTexelSize, -fTexelSize, 0.0f, 0.0f);
	vTexCoords[2][5] = In.TexShadow2 + float4(fTexelSize, -fTexelSize, 0.0f, 0.0f);
	vTexCoords[2][7] = In.TexShadow2 + float4(-fTexelSize, fTexelSize, 0.0f, 0.0f);
	vTexCoords[2][8] = In.TexShadow2 + float4(fTexelSize, fTexelSize, 0.0f, 0.0f);

	float S0 = (depth0 - SHADOW_EPSILON);
	float S1 = (depth1 - SHADOW_EPSILON);
	float S2 = (depth2 - SHADOW_EPSILON);

	float fShadowTerms[9];
	float fShadowTerm = 0.0f;
	for (int i = 0; i < 9; i++)
	{
		const float2 uv0 = vTexCoords[0][i].xy / vTexCoords[0][i].w;
		const float2 uv1 = vTexCoords[1][i].xy / vTexCoords[1][i].w;
		const float2 uv2 = vTexCoords[2][i].xy / vTexCoords[2][i].w;
		const float D0 = txShadow0.Sample(samShadow, uv0).r;
		const float D1 = txShadow1.Sample(samShadow, uv1).r;
		const float D2 = txShadow2.Sample(samShadow, uv2).r;

		fShadowTerms[i] = ((D0 < S0) || (D1 < S1) || (D2 < S2)) ? 0.1f : 1.0f;
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

	float4 Out = float4(color.xyz, gMtrl_Diffuse.w) * txDiffuse.Sample(samLinear, In.Tex);
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
	//return float4(0,0,0,1);
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

