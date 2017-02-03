

struct Light
{
	float3 dir;                     // world space direction
	float3 pos;                     // world space position
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float spotInnerCone;            // spot light inner cone (theta) angle
	float spotOuterCone;            // spot light outer cone (phi) angle
	float radius;                   // applies to point and spot lights only
};

struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 emissive;
	float4 specular;
	float shininess;
};


Light light;
Material material;
float3 vEyePos;
matrix mWorld;
matrix g_mWVP;
matrix mVP;



void VS_pass0( float4 inPosition : POSITION
   	     , float4 inDiffuse : COLOR0 
	     , out float4 oPosition : POSITION
	     , out float4 oDiffuse : COLOR0 
	     )
{
	oPosition = mul( inPosition, g_mWVP );
	oDiffuse = inDiffuse;
}


float4  PS_pass0(float4 inDiffuse : COLOR0) : COLOR0
{
	return inDiffuse;
}


technique Scene
{
    pass P0
    {
        VertexShader = compile vs_3_0 VS_pass0();
	PixelShader = compile ps_3_0 PS_pass0();

    }
}


