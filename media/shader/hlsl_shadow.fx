
float4x4 g_mWorld;
float4x4 g_mVP;
float4x4 g_mView;
float4x4 g_mProj;                   // Projection matrix

float3 g_vEyePos;
float shininess = 32;

float3   g_vLightView;              // View space light position/direction
float4   g_vLightColor;             // Light color
float4   g_vShadowColor;            // Shadow volume color (for visualization)

float    g_fFarClip;                // Z of far clip plane


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


Light g_light;
Material g_material;




texture g_colorMapTexture;
sampler colorMap = sampler_state
{
    Texture = <g_colorMapTexture>;
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
	float4x4 mVP = mul(g_mView, g_mProj);
	float4x4 mWVP = mul(g_mWorld, mVP);
	oPos = mul( vPos, mWVP );
	oTex = Tex;
}


float4 PS_Ambient(float2 Tex : TEXCOORD1) : COLOR0
{
	float4 Out = g_light.ambient * g_material.diffuse;
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

	float4x4 mVP = mul(g_mView, g_mProj);
	float4x4 mWVP = mul(g_mWorld, mVP);
	Out.Pos = mul( Pos, mWVP );

	float3 N = normalize( mul( Normal, (float3x3)g_mWorld ) );
	
	Out.N = N;
	Out.Eye = g_vEyePos - mul(Pos, g_mWorld).xyz;
	Out.Tex = Tex;

	return Out;
}


float4 PS_pass0(VS_OUTPUT In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 Out = 	g_light.ambient * g_material.ambient
			+ g_light.diffuse * g_material.diffuse * max(0, dot(N,L));
			+ g_light.specular * pow( max(0, dot(N,H)), shininess);

	Out = Out * tex2D(colorMap, In.Tex);
	return Out;
}




void VS_Shadow(
	float4 vPos    : POSITION,
	float3 vNormal : NORMAL,
	out float4 oPos : POSITION )
{
	float4x4 mVP = mul(g_mView, g_mProj);
	float4x4 mWV = mul(g_mWorld, g_mView);
	float4x4 mWVP = mul(g_mWorld, mVP);
	float3 N = mul( vNormal, (float3x3)mWV );
	float4 PosView = mul( vPos, mWV );
	float3 LightVecView = PosView - g_vLightView;
	//oPos = mul( vPos, mWVP );
	//return;

        if (dot(N, -LightVecView) > 0.0f)
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
        VertexShader = compile vs_3_0 VS_Ambient();
	PixelShader  = compile ps_3_0 PS_Ambient();
        StencilEnable = false;
        ZFunc = LessEqual;
    }
}



technique Scene
{
    pass P0
    {
        VertexShader = compile vs_3_0 VS_pass0();
	PixelShader  = compile ps_3_0 PS_pass0();

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
        VertexShader = compile vs_3_0 VS_Shadow();
	PixelShader  = compile ps_3_0 PS_Shadow();

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
        VertexShader = compile vs_3_0 VS_Shadow();
	PixelShader  = compile ps_3_0 PS_Shadow();

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


