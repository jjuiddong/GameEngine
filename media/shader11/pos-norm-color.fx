
#define Instancing		true
#define NotInstancing	false


//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse	: register(t0);
Texture2D txBump	: register(t1);
Texture2D txShadow0	: register(t2);
Texture2D txShadow1	: register(t3);
Texture2D txShadow2	: register(t4);


cbuffer cbPerFrame : register( b0 )
{
	matrix gWorld;
	matrix gView;
	matrix gProjection;
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

cbuffer cbPerFrameInstancing : register( b3 )
{
	matrix gWorldInst[100];
}


cbuffer cbClipPlane : register(b4)
{
	float4	gClipPlane;
}



//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : TEXCOORD0;
	float4 Color : COLOR0;
	float3 toEye : TEXCOORD2;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float4 Color : COLOR
	, uniform bool IsInstancing
	 )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, gWorld );
    output.toEye = gEyePosW - output.Pos;
    output.Pos = mul( output.Pos, gView );
    output.Pos = mul( output.Pos, gProjection );
    output.Normal = normalize( mul(Normal, (float3x3)gWorld) );
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

	float4 Out = color * In.Color;
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
	, float4 Color : COLOR
	, uint instID : SV_InstanceID
	, uniform bool IsInstancing
)
{
	VS_BUILDSHADOW_OUTPUT output = (VS_BUILDSHADOW_OUTPUT)0;
	const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;

	output.Pos = mul(Pos, mWorld);
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
        SetVertexShader( CompileShader( vs_5_0, VS(NotInstancing) ) );
	SetGeometryShader( NULL );
        SetHullShader(NULL);
      	SetDomainShader(NULL);
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}


technique11 ShadowMap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS(NotInstancing)));
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
		SetVertexShader(CompileShader(vs_5_0, VS_BuildShadowMap(NotInstancing)));
		SetGeometryShader(NULL);
 	        SetHullShader(NULL);
        	SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_BuildShadowMap()));
	}
}


