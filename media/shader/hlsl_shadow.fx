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



void VS_Ambient( float4 vPos : POSITION
		,float2 Tex : TEXCOORD0
                , out float4 oPos : POSITION
		, out float2 oTex : TEXCOORD1 )
{
	float4x4 mWVP = mul(mWorld, mVP);
	oPos = mul( vPos, mWVP );
	oTex = Tex;
}


float4 PS_Ambient(float2 Tex : TEXCOORD1) : COLOR0
{
	float4 Out = light.ambient * material.diffuse;
	Out = Out * tex2D(colorMap, Tex);
	return Out;
}




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




void VS_Shadow(
	float4 vPos    : POSITION,
	float3 vNormal : NORMAL,
	out float4 oPos : POSITION )
{
	float4x4 mWV = mul(mWorld, g_mView);
	float4x4 mWVP = mul(mWorld, mVP);
	float3 N = mul( vNormal, (float3x3)mWV );
	float4 PosView = mul( vPos, mWV );
	float3 LightVecView = PosView - g_vLightView;
        if( dot( N, -LightVecView ) < 0.0f )
	{
	        if( PosView.z > g_vLightView.z )
        	    PosView.xyz += LightVecView * ( g_fFarClip - PosView.z ) / LightVecView.z;
	        else
	            PosView = float4( LightVecView, 0.0f );

        	oPos = mul( PosView, g_mProj );
	} 
	else
	{
        	oPos = mul( vPos, mWVP );
	}
}



float4 PS_Shadow() : COLOR0
{
	return float4( g_vShadowColor.xyz, 0.1f );
}



technique Ambient
{
    pass P0
    {
        VertexShader = compile vs_2_0 VS_Ambient();
	PixelShader  = compile ps_2_0 PS_Ambient();
        StencilEnable = false;
        ZFunc = LessEqual;
    }
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


technique Shadow
{
    pass P0
    {
        VertexShader = compile vs_2_0 VS_Shadow();
	PixelShader  = compile ps_2_0 PS_Shadow();

        CullMode = None;
    
	// Disable writing to the frame buffer
        AlphaBlendEnable = true;
      	SrcBlend = Zero;
        DestBlend = One;
        
	// Disable writing to depth buffer
        ZEnable = true;
        ZWriteEnable = false;
        ZFunc = Less;
        
	// Setup stencil states
        TwoSidedStencilMode = true;
        StencilEnable = true;
        StencilRef = 1;
        StencilMask = 0xFFFFFFFF;
        StencilWriteMask = 0xFFFFFFFF;
        Ccw_StencilFunc = Always;
        Ccw_StencilZFail = Keep;
        Ccw_StencilPass = Decr;
        StencilFunc = Always;
        StencilZFail = Keep;
        StencilPass = Incr;
    }
}



technique ShowShadow
{
    pass P0
    {
        VertexShader = compile vs_2_0 VS_Shadow();
	PixelShader  = compile ps_2_0 PS_Shadow();

        CullMode = None;
    
	// Disable writing to the frame buffer
        AlphaBlendEnable = true;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
        
	// Disable writing to depth buffer
        ZEnable = true;
        ZWriteEnable = false;
        ZFunc = Less;
        
	// Setup stencil states
        TwoSidedStencilMode = true;
        StencilEnable = true;
        StencilRef = 1;
        StencilMask = 0xFFFFFFFF;
        StencilWriteMask = 0xFFFFFFFF;
        Ccw_StencilFunc = Always;
        Ccw_StencilZFail = Keep;
        Ccw_StencilPass = Decr;
        StencilFunc = Always;
        StencilZFail = Keep;
        StencilPass = Incr;
    }
}


