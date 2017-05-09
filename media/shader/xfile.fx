//
// 2017-05-01, jjuiddong
// xfile shader
//
// 2017-05-09
//	- shadow map


float4x4 g_mView;
float4x4 g_mProj;
float4x4 g_mWorld;
float4x4 g_mWIT;
float4x4 g_mWVPT; // ShadowMap Transform
float3 g_vEyePos; // Eye Position in world space


//#define SMAP_SIZE 1024
#define SMAP_SIZE 2048
#define SHADOW_EPSILON 0.001f

float4x4 g_mViewToLightProj;  // Transform from view space to light projection space
float3 g_vLightPos; // Light position in view space
float3 g_vLightDir; // Light direction in view space
float4 g_vLightAmbient = float4( 0.3f, 0.3f, 0.3f, 1.0f );  // Use an ambient light of 0.3
float4 g_vMaterial = float4(1,1,1,1);

float g_shininess = 90;
float g_fFarClip = 10000;

float4x3 g_mPalette[ 64];


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


texture g_colorMapTexture;
sampler colorMap = sampler_state
{
    Texture = <g_colorMapTexture>;
    MinFilter = Anisotropic;
    MagFilter = Anisotropic;
    MipFilter = Linear;
    AddressU  = WRAP;
    AddressV  = WRAP;
};


texture g_shadowMapTexture;
sampler shadowMap = sampler_state
{
    	Texture = <g_shadowMapTexture>;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Clamp;
	AddressV = Clamp;
};




struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
	float3 Eye : TEXCOORD1;
};


struct VS_OUTPUT_SHADOW
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
	float4 TexShadow : TEXCOORD1;
	float3 Eye : TEXCOORD2;
	float4 vPos : TEXCOORD3;
	float4 vPosLight : TEXCOORD4;
};



//-----------------------------------------------------------------------------------
// WireFrame

void VS_Wire(
	float4 Pos : POSITION,
	out float4 oPos : POSITION
)
{
	float4x4 mVP = mul(g_mView, g_mProj);
	float3 worldPos = mul(Pos, g_mWorld).xyz;
	oPos = mul( float4(worldPos,1), mVP );
}


float4 PS_Wire() : COLOR
{
	return float4(1,0,0,1);
}




//-----------------------------------------------------------------------------------
// Ambient

VS_OUTPUT VS(
	float4 Pos : POSITION,
	float3 Normal : NORMAL,
	float2 Tex : TEXCOORD0
)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
    
	float4x4 mVP = mul(g_mView, g_mProj);
	float4x4 mWVP = mul(g_mWorld, mVP);
	float3 N = normalize( mul(Normal, (float3x3)g_mWorld) );

	Out.Pos = mul( Pos, mWVP );
	Out.Normal = N;
	Out.Eye = g_vEyePos - mul(Pos, g_mWorld).xyz;
	Out.Tex = Tex;

	return Out;
}




float4 PS_Ambient(VS_OUTPUT In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.Normal);

	float4 color = 	g_light.ambient * g_material.ambient
			+ g_light.diffuse * g_material.diffuse * 0.2
			+ (g_light.diffuse * g_material.diffuse * max(0, dot(N,L))) * 0.1f;

	float4 Out = color * tex2D(colorMap, In.Tex);
	return Out;
}





//-----------------------------------------------------------------------------------
// Shadow

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
	float4 LightPos = mul( g_light.pos, g_mView );
	float3 LightVecView = PosView - LightPos;

        if (dot(N, -LightVecView) > 0.0f)
	{
	        if( PosView.z > LightPos.z )
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


float4 PS_Shadow() : COLOR
{
	return float4(0,1,0,1);
}




void VS_ShadowMap(
	float4 Pos : POSITION
	, float3 Normal : NORMAL
        , out float4 oPos : POSITION
        , out float2 Depth : TEXCOORD0
)
{
	float4x4 mVP = mul(g_mView, g_mProj);
	float4x4 mWVP = mul(g_mWorld, mVP);

	oPos = mul( Pos, mWVP );
	Depth.xy = oPos.zw;
}



void PS_ShadowMap(
		float2 Depth : TEXCOORD0
		, out float4 Color : COLOR 
)
{
	Color = Depth.x / Depth.y;

	//Color = float4(1,1,1,1);
	//Color = Depth.x * 1;
}





//-----------------------------------------------------------------------------------
// Scene

float4 PS_Scene(VS_OUTPUT In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.Normal);

	float4 color  = g_light.diffuse * g_material.diffuse * max(0, dot(N,L))
			+ g_light.specular * pow( max(0, dot(N,H)), g_shininess);

	float4 Out = color * tex2D(colorMap, In.Tex);
	return Out;
}


float4 PS_Scene_NoShadow(VS_OUTPUT In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.Normal);

	float4 color  = g_light.ambient * g_material.ambient
			+ g_light.diffuse * g_material.diffuse * max(0, dot(N,L))
			+ g_light.specular * pow( max(0, dot(N,H)), g_shininess);

	float4 Out = color * tex2D(colorMap, In.Tex);
	return Out;
}



//--------------------------------------------------------------------------------
// ShadowMap Render

VS_OUTPUT_SHADOW VS_Scene_ShadowMap(
	float4 Pos : POSITION,
	float3 Normal : NORMAL,
	float2 Tex : TEXCOORD0
)
{
	VS_OUTPUT_SHADOW Out = (VS_OUTPUT_SHADOW)0;

	float4x4 mWV = mul(g_mWorld, g_mView);
	float4x4 mVP = mul(g_mView, g_mProj);
	float4x4 mWVP = mul(g_mWorld, mVP);
	float3 N = normalize( mul(Normal, (float3x3)mWV) );

	Out.Pos = mul( Pos, mWVP );
	Out.Normal = N;
	Out.Eye = g_vEyePos - mul(Pos, g_mWorld).xyz;
	Out.Tex = Tex;
	Out.TexShadow = mul( Pos, g_mWVPT );
	Out.vPos = mul( Pos, mWV);
    	Out.vPosLight = mul( Out.vPos, g_mViewToLightProj );

	return Out;
}


float4 PS_Scene_ShadowMap(VS_OUTPUT_SHADOW In) : COLOR
{
	float4 Diffuse;

	// vLight is the unit vector from the light to this pixel
	float3 vLight = normalize( float3( In.vPos - g_vLightPos ) );

	// Compute diffuse from the light
        // Pixel is in lit area. Find out if it's
        // in shadow using 2x2 percentage closest filtering

        //transform from RT space to texture space. (-1 ~ +1) -> (0 ~ 1)
        float2 ShadowTexC = 0.5 * In.vPosLight.xy / In.vPosLight.w + float2( 0.5f, 0.5f );
        ShadowTexC.y = 1.0f - ShadowTexC.y;

	//float2 ShadowTexC = In.TexShadow.xy;
	//float4 ShadowTexC = In.TexShadow;

        // transform to texel space
        float2 texelpos = SMAP_SIZE * ShadowTexC;
        
        // Determine the lerp amounts, return 0 ~ 1 value
        float2 lerps = frac( texelpos );

	float z = (In.vPosLight.z / In.vPosLight.w) * 1;

        //read in bilerp stamp, doing the shadow checks
        float sourcevals[1];
	//sourcevals[0] = ((tex2D( shadowMap, ShadowTexC ) + SHADOW_EPSILON) < z)? 0.0f: 1.0f;  
        //sourcevals[1] = (tex2D( shadowMap, ShadowTexC + float2(1.0/SMAP_SIZE, 0) ) + SHADOW_EPSILON < z)? 0.0f: 1.0f;  
        //sourcevals[2] = (tex2D( shadowMap, ShadowTexC + float2(0, 1.0/SMAP_SIZE) ) + SHADOW_EPSILON < z)? 0.0f: 1.0f;  
        //sourcevals[3] = (tex2D( shadowMap, ShadowTexC + float2(1.0/SMAP_SIZE, 1.0/SMAP_SIZE) ) + SHADOW_EPSILON < z)? 0.0f: 1.0f;  

	//sourcevals[4] = (tex2D( shadowMap, ShadowTexC + float2(-1.0/SMAP_SIZE, 0) ) + SHADOW_EPSILON < z)? 0.0f: 1.0f;  
        //sourcevals[5] = (tex2D( shadowMap, ShadowTexC + float2(0, -1.0/SMAP_SIZE) ) + SHADOW_EPSILON < z)? 0.0f: 1.0f;  
        //sourcevals[6] = (tex2D( shadowMap, ShadowTexC + float2(-1.0/SMAP_SIZE, -1.0/SMAP_SIZE) ) + SHADOW_EPSILON < z)? 0.0f: 1.0f;  


        // lerp between the shadow values to calculate our light amount
        //float LightAmount = lerp( lerp( sourcevals[0], sourcevals[1], lerps.x ),
                                  //lerp( sourcevals[2], sourcevals[3], lerps.x ),
                                  //lerps.y );

	//float LightAmount = (sourcevals[0] + sourcevals[1] + sourcevals[2] + sourcevals[3]
			//+ sourcevals[4] + sourcevals[5] + sourcevals[6]) / 6.f;

	//float LightAmount = z;
	//float LightAmount = sourcevals[0];
	//LightAmount = 1;


        // float sourcevals[0];
	sourcevals[0] = ((tex2D( shadowMap, ShadowTexC ) + SHADOW_EPSILON) < z)? 0.0f: 1.0f;  
	float LightAmount = sourcevals[0];


        // lerp between the shadow values to calculate our light amount
        //float LightAmount = lerp( lerp( sourcevals[0], sourcevals[1], lerps.x ),
                                  //lerp( sourcevals[2], sourcevals[3], lerps.x ),
                                  //lerps.y );

	//float LightAmount = (sourcevals[0] + sourcevals[1] + sourcevals[2] + sourcevals[3]
			//+ sourcevals[4] + sourcevals[5] + sourcevals[6]) / 6.f;



        // Light it
        Diffuse = ( saturate( dot(-vLight, In.Normal) ) * LightAmount * ( 1 - g_vLightAmbient ) + g_vLightAmbient )
                  * g_vMaterial;

    	return tex2D( colorMap, In.Tex ) * Diffuse;
	//return tex2Dproj( shadowMap, ShadowTexC );
	//return tex2D( shadowMap, ShadowTexC );
	//return Diffuse;
}




//-----------------------------------------------------------------------------------
// Unlit

float4 PS_Unlit(VS_OUTPUT In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.Normal);

	float4 color  = g_light.ambient * g_material.ambient
			+ g_light.diffuse * g_material.diffuse * (max(0, dot(N,L)) + 0.5)
			+ g_light.specular * pow( max(0, dot(N,H)), g_shininess);

	return color;
}




technique Ambient
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader  = compile ps_3_0 PS_Ambient();


		//CullMode = CCW;
		//FillMode = solid;
		//AlphaBlendEnable = false;
	        StencilEnable = false;
	        ZFunc = LessEqual;
	}
}



technique Shadow
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS_Shadow();
		PixelShader  = compile ps_3_0 PS_Shadow();

	        CullMode = None;
		//FillMode = solid;
    
		// Disable writing to the frame buffer
	        AlphaBlendEnable = true;
      		SrcBlend = Zero;
		DestBlend = One;
	        //SrcBlend = SrcAlpha;
	        //DestBlend = InvSrcAlpha;
        
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



technique Scene
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader  = compile ps_3_0 PS_Scene();


		//CullMode = CCW;
		//FillMode = solid;
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



technique Scene_NoShadow
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader  = compile ps_3_0 PS_Scene_NoShadow();
	}
}




technique ShadowMap
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS_ShadowMap();
		PixelShader  = compile ps_3_0 PS_ShadowMap();
	}
}


technique Scene_ShadowMap
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS_Scene_ShadowMap();
		PixelShader  = compile ps_3_0 PS_Scene_ShadowMap();
	}
}



technique WireFrame
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS_Wire();
		PixelShader  = compile ps_3_0 PS_Wire();


		CullMode = CCW;
		//FillMode = wireframe;
        	ZEnable = true;
	        ZFunc = LessEqual;
	        StencilEnable = false;
	        AlphaBlendEnable = false;
	}
}


technique Unlit
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader  = compile ps_3_0 PS_Unlit();

		CullMode = CCW;
		//FillMode = solid;
        	ZEnable = true;
	        ZFunc = LessEqual;
	        StencilEnable = false;
	        AlphaBlendEnable = false;
	}
}


