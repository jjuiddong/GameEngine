float4x4 mWorld;
float4x4 mVP;		// 로컬에서 투영공간으로의 좌표변환
float4x4 mWIT;
float3 vLightDir = {1, -1, 1};
float3 vEyePos;

float4   g_vAmbient;                // Ambient light color
float3   g_vLightView;              // View space light position/direction
float4   g_vLightColor;             // Light color
float4   g_vShadowColor;            // Shadow volume color (for visualization)
float4   g_vMatColor;               // Color of the material

float4x4 g_mWorldView;              // World * View matrix
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
};



void VS_Ambient( float4 vPos : POSITION,
                       out float4 oPos : POSITION )
{
	float4x4 mWVP = mul(mWorld, mVP);
	oPos = mul( vPos, mWVP );
}


float4 PS_Ambient() : COLOR0
{
    return g_vAmbient * g_vMatColor;
}




void VS_pass0(
	float4 Pos    : POSITION,
	float3 Normal : NORMAL,
	out float4 oPos : POSITION,
	out float4 oDiffuse : TEXCOORD0
)
{
	float4x4 mWVP = mul(mWorld, mVP);
	oPos = mul( Pos, mWVP );

	float3 L = -normalize( vLightDir );
	float3 N = mul( Normal, (float3x3)mWorld );
	oDiffuse = I_d * K_d * max(0, dot(N,L));
}


float4 PS_pass0(float4 Diffuse : TEXCOORD0) : COLOR
{
	float4 Out;
	Out = Diffuse;
	return Out;
}



void VS_Shadow(
	float4 vPos    : POSITION,
	float3 vNormal : NORMAL,
	out float4 oPos : POSITION )
{
	float3 N = mul( vNormal, (float3x3)g_mWorldView );
	float4 PosView = mul( vPos, g_mWorldView );
	float3 LightVecView = PosView - g_vLightView;
        if( dot( N, LightVecView ) < 0.0f )
	{
	        if( PosView.z > g_vLightView.z )
        	    PosView.xyz += LightVecView * ( g_fFarClip - PosView.z ) / LightVecView.z;
	        else
	            PosView = float4( LightVecView, 0.0f );

        	oPos = mul( PosView, g_mProj );
	} 
	else
	{
        	oPos = mul( vPos, g_mWorldViewProjection );
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


