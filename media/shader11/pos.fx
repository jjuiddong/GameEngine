
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 )
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
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
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT In ) : SV_Target
{
	//return gLight_Diffuse * gMtrl_Diffuse;
	return gMtrl_Diffuse;
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

