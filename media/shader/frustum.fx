//
// 2017-05-20, jjuiddong
// frustum shader
// Pos + Color
//


float4x4 g_mView;
float4x4 g_mProj;
float4x4 g_mWorld;
float4x4 g_mWIT;
float3 g_vEyePos;
float g_shininess = 90;
float g_fFarClip = 10000;



struct Light
{
	float3 dir;	// world space direction
	float3 pos;	// world space position
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float spotInnerCone;	// spot light inner cone (theta) angle
	float spotOuterCone;	// spot light outer cone (phi) angle
	float radius;           // applies to point and spot lights only
};

struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 emissive;
	float4 specular;
	float shininess;
};

Light g_light;
Material g_material;


void VS(
	float4 Pos : POSITION
	, float4 inDiffuse : COLOR0
	, out float4 oPos : POSITION
)
{
	float4x4 mVP = mul(g_mView, g_mProj);
	float3 worldPos = mul(Pos, g_mWorld).xyz;

	oPos = mul( float4(worldPos,1), mVP );
}


float4 PS_Scene(float4 Pos : POSITION) : COLOR
{
	return float4(g_material.diffuse.xyz, 0.5f);
}




technique Scene
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader  = compile ps_3_0 PS_Scene();

		CullMode = CCW;
		FillMode = solid;
        	ZEnable = true;
	        StencilEnable = false;
	        AlphaBlendEnable = true;
	        BlendOp = Add;
	        SrcBlend = SrcAlpha;
	        DestBlend = InvSrcAlpha;
	}
}

