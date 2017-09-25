
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );


cbuffer ConstantBuffer : register( b0 )
{
	matrix gWorld;
	matrix gView;
	matrix gProjection;
	matrix gLightView[3];
	matrix gLightProj[3];
	matrix gLightTT;
	float3 gEyePosW;
}

cbuffer cbClipPlane : register(b1)
{
	float4	gClipPlane;
}



//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD1;
	float clip : SV_ClipDistance0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION
	, float2 Tex : TEXCOORD0
	 )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, gWorld );
    output.Pos = mul( output.Pos, gView );
    output.Pos = mul( output.Pos, gProjection );
    output.Tex = Tex;
	output.clip = dot(mul(Pos, gWorld), gClipPlane);
    return output;
}


VS_OUTPUT VS_Skybox( float4 Pos : POSITION
	, float2 Tex : TEXCOORD0
	 )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
	matrix mView = gView;
	mView._41 = 0;
	mView._42 = -0.4;
	mView._43 = 0;

    output.Pos = mul( Pos, gWorld );
    output.Pos = mul( output.Pos, mView );
    output.Pos = mul( output.Pos, gProjection );
    output.Tex = Tex;
	output.clip = dot(mul(Pos, gWorld), gClipPlane);
    return output;
}



//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    return txDiffuse.Sample( samLinear, input.Tex );
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


technique11 Skybox
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Skybox()));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}

