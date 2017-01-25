float4x4 mWorld;
float4x4 mVP;		// 로컬에서 투영공간으로의 좌표변환
float4x4 mWIT;
float3 vLightDir = {1, -1, 1};
float3 vEyePos;
float shininess = 32;

float4   g_vAmbient;                // Ambient light color
float3   g_vLightView;              // View space light position/direction
float4   g_vLightColor;             // Light color
float4   g_vShadowColor;            // Shadow volume color (for visualization)
float4   g_vMatColor;               // Color of the material

float4x4 g_mWorldView;              // World * View matrix
float4x4 g_mView;
float4x4 g_mProj;                   // Projection matrix
float4x4 g_mWorldViewProjection;    // World * View * Projection matrix

float    g_fFarClip;                // Z of far clip plane


// 광원 밝기.
float4 I_a = {0.0f, 0.0f, 0.0f, 0.0f}; // ambient
float4 I_d = {1.f, 1.f, 1.f, 0.0f}; // diffuse
float4 I_s = {1.f, 1.f, 1.f, 0.0f}; // diffuse

// 반사율
float4 K_a = {1.0f, 1.0f, 1.0f, 1.0f}; // ambient 
float4 K_d = {1.0f, 1.0f, 1.0f, 1.0f}; // diffuse


struct Light
{
	float3 dir;				// world space direction
	float3 pos;				// world space position
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


Light light;
Material material;




texture colorMapTexture;
sampler colorMap = sampler_state
{
    Texture = <colorMapTexture>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Wrap;
    AddressV = Wrap;
};


struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float4 Diffuse : COLOR0;
	float2 Tex : TEXCOORD0;
	float3 Eye : TEXCOORD1;
	float3 N : TEXCOORD2;
};



VS_OUTPUT VS_pass0(
	float4 Pos    : POSITION
	,float3 Normal : NORMAL
	,float2 Tex : TEXCOORD0
	)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	float4x4 mWVP = mul(mWorld, mVP);
	Out.Pos = mul( Pos, mWVP );

	float3 N = normalize( mul( Normal, (float3x3)mWorld ) );
	
	Out.N = N;
	Out.Eye = vEyePos - mul(Pos, mWorld).xyz;
	Out.Tex = Tex;

	return Out;
}


float4 PS_pass0(VS_OUTPUT In) : COLOR
{
	float3 L = -light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 Out = 	light.ambient * material.ambient
			+ light.diffuse * material.diffuse * max(0, dot(N,L));
			+ light.specular * pow( max(0, dot(N,H)), shininess);

	Out = Out * tex2D(colorMap, In.Tex);
	return Out;
}



technique Scene
{
    pass P0
    {
        VertexShader = compile vs_2_0 VS_pass0();
	PixelShader  = compile ps_2_0 PS_pass0();

        ZEnable = true;
        ZFunc = LessEqual;
        StencilEnable = true;
        AlphaBlendEnable = true;
        BlendOp = Add;
        SrcBlend = One;
        DestBlend = One;
        StencilRef = 1;
        StencilFunc = Greater;
        StencilPass = Keep;
    }
}


