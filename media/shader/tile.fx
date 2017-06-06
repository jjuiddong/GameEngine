//
// 2017-05-01, jjuiddong
// xfile shader
// Pos + Normal + Tex
//


float4x4 g_mView;
float4x4 g_mProj;
float4x4 g_mWorld;
float4x4 g_mWIT;
float4x4 g_mVPT; // ShadowMap Transform, = light view x light proj x uv transform
float4x4 g_mLVP; // ShadowMap Transform, Light View Projection, = light view x light proj

float4x4 g_mLightView1;
float4x4 g_mLightView2;
float4x4 g_mLightView3;
float4x4 g_mLightProj1;
float4x4 g_mLightProj2;
float4x4 g_mLightProj3;
float4x4 g_mLightTT1;
float4x4 g_mLightTT2;
float4x4 g_mLightTT3;

float3 g_vEyePos;
float g_shininess = 90;
float g_fFarClip = 10000;
float g_uvFactor = 1.f;
float4x3 g_mPalette[ 64];

#define SHADOW_EPSILON 0.00001f



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
    MinFilter = None;
    MagFilter = None;
    MipFilter = Linear;

    AddressU  = Wrap;
    AddressV  = Wrap;
};


texture g_shadowMapTexture1;
texture g_shadowMapTexture2;
texture g_shadowMapTexture3;
sampler shadowMap1 = sampler_state
{
    	Texture = <g_shadowMapTexture1>;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Border;
	AddressV = Border;

	BorderColor = 0xffffffff;
};

sampler shadowMap2 = sampler_state
{
    	Texture = <g_shadowMapTexture2>;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Border;
	AddressV = Border;

	BorderColor = 0xffffffff;
};

sampler shadowMap3 = sampler_state
{
    	Texture = <g_shadowMapTexture3>;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Border;
	AddressV = Border;

	BorderColor = 0xffffffff;
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
	float4 TexShadow1 : TEXCOORD1;
	float4 TexShadow2 : TEXCOORD2;
	float4 TexShadow3 : TEXCOORD3;
	float3 Eye : TEXCOORD4;
	float4 Depth1 : TEXCOORD5;
	float2 Depth2 : TEXCOORD6;
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
	float3 worldPos = mul(Pos, g_mWorld).xyz;
	float3 N = normalize( mul(Normal, (float3x3)g_mWorld) );

	Out.Pos = mul( float4(worldPos,1), mVP );
	Out.Normal = N;
	Out.Eye = g_vEyePos - mul(Pos, g_mWorld).xyz;
	Out.Tex = Tex * g_uvFactor;

	return Out;
}


float4 PS_Ambient(VS_OUTPUT In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.Normal);

	float4 color = 	g_light.ambient * g_material.ambient
			+ (g_light.diffuse * g_material.diffuse * max(0, dot(N,L))) * 0.1f;

	float4 Out = color * tex2D(colorMap, In.Tex);
	return Out;
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
	float4 LightPos = mul( float4(g_light.pos,1), mWV );
	float3 LightVecView = PosView.xyz - LightPos.xyz;

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
	float4 wPos = mul(Pos, g_mWorld);

	Out.Pos = mul( Pos, mWVP );
	Out.Normal = N;
	Out.Eye = g_vEyePos - mul(Pos, g_mWorld).xyz;
	Out.Tex = Tex * g_uvFactor;
	//Out.TexShadow = mul( wPos, g_mVPT );
	//Out.vPos = mul( Pos, mWV);
	//Out.Depth = mul( wPos, g_mLVP ).zw;

	float4x4 mLVP1 = mul(g_mLightView1, g_mLightProj1);
	float4x4 mVPT1 = mul( mLVP1, g_mLightTT1);
	Out.TexShadow1 = mul( wPos, mVPT1 );
	Out.Depth1.xy = mul( wPos, mLVP1 ).zw;

	float4x4 mLVP2 = mul(g_mLightView2, g_mLightProj2);
	float4x4 mVPT2 = mul( mLVP2, g_mLightTT2);
	Out.TexShadow2 = mul( wPos, mVPT2 );
	Out.Depth1.zw = mul( wPos, mLVP2 ).zw;

	float4x4 mLVP3 = mul(g_mLightView3, g_mLightProj3);
	float4x4 mVPT3 = mul( mLVP3, g_mLightTT3);
	Out.TexShadow3 = mul( wPos, mVPT3 );
	Out.Depth2.xy = mul( wPos, mLVP3 ).zw;

	return Out;
}


float4 PS_Scene_ShadowMap(VS_OUTPUT_SHADOW In) : COLOR
{
	float4 vTexCoords[3][9];
	float fTexelSize = 1.0f / 1024.0f;
	float depth1 = min(In.Depth1.x / In.Depth1.y, 1.0);
	float depth2 = min(In.Depth1.z / In.Depth1.w, 1.0);
	float depth3 = min(In.Depth2.x / In.Depth2.y, 1.0);

	// Generate the tecture co-ordinates for the specified depth-map size
	// 4 3 5
	// 1 0 2
   	// 7 6 8
   	vTexCoords[0][0] = In.TexShadow1;
   	vTexCoords[0][1] = In.TexShadow1 + float4( -fTexelSize, 0.0f, 0.0f, 0.0f );
   	vTexCoords[0][2] = In.TexShadow1 + float4(  fTexelSize, 0.0f, 0.0f, 0.0f );
   	vTexCoords[0][3] = In.TexShadow1 + float4( 0.0f, -fTexelSize, 0.0f, 0.0f );
   	vTexCoords[0][6] = In.TexShadow1 + float4( 0.0f,  fTexelSize, 0.0f, 0.0f );
   	vTexCoords[0][4] = In.TexShadow1 + float4( -fTexelSize, -fTexelSize, 0.0f, 0.0f );
   	vTexCoords[0][5] = In.TexShadow1 + float4(  fTexelSize, -fTexelSize, 0.0f, 0.0f );
   	vTexCoords[0][7] = In.TexShadow1 + float4( -fTexelSize,  fTexelSize, 0.0f, 0.0f );
   	vTexCoords[0][8] = In.TexShadow1 + float4(  fTexelSize,  fTexelSize, 0.0f, 0.0f );

   	vTexCoords[1][0] = In.TexShadow2;
   	vTexCoords[1][1] = In.TexShadow2 + float4( -fTexelSize, 0.0f, 0.0f, 0.0f );
   	vTexCoords[1][2] = In.TexShadow2 + float4(  fTexelSize, 0.0f, 0.0f, 0.0f );
   	vTexCoords[1][3] = In.TexShadow2 + float4( 0.0f, -fTexelSize, 0.0f, 0.0f );
   	vTexCoords[1][6] = In.TexShadow2 + float4( 0.0f,  fTexelSize, 0.0f, 0.0f );
   	vTexCoords[1][4] = In.TexShadow2 + float4( -fTexelSize, -fTexelSize, 0.0f, 0.0f );
   	vTexCoords[1][5] = In.TexShadow2 + float4(  fTexelSize, -fTexelSize, 0.0f, 0.0f );
   	vTexCoords[1][7] = In.TexShadow2 + float4( -fTexelSize,  fTexelSize, 0.0f, 0.0f );
   	vTexCoords[1][8] = In.TexShadow2 + float4(  fTexelSize,  fTexelSize, 0.0f, 0.0f );

   	vTexCoords[2][0] = In.TexShadow3;
   	vTexCoords[2][1] = In.TexShadow3 + float4( -fTexelSize, 0.0f, 0.0f, 0.0f );
   	vTexCoords[2][2] = In.TexShadow3 + float4(  fTexelSize, 0.0f, 0.0f, 0.0f );
   	vTexCoords[2][3] = In.TexShadow3 + float4( 0.0f, -fTexelSize, 0.0f, 0.0f );
   	vTexCoords[2][6] = In.TexShadow3 + float4( 0.0f,  fTexelSize, 0.0f, 0.0f );
   	vTexCoords[2][4] = In.TexShadow3 + float4( -fTexelSize, -fTexelSize, 0.0f, 0.0f );
   	vTexCoords[2][5] = In.TexShadow3 + float4(  fTexelSize, -fTexelSize, 0.0f, 0.0f );
   	vTexCoords[2][7] = In.TexShadow3 + float4( -fTexelSize,  fTexelSize, 0.0f, 0.0f );
   	vTexCoords[2][8] = In.TexShadow3 + float4(  fTexelSize,  fTexelSize, 0.0f, 0.0f );


	float S1 = (depth1 - SHADOW_EPSILON);
	float S2 = (depth2 - SHADOW_EPSILON);
	float S3 = (depth3 - SHADOW_EPSILON);

        float fShadowTerms[9];
	float fShadowTerm = 0.0f;
   	for( int i = 0; i < 9; i++ )
   	{
	  	float D1 = tex2Dproj( shadowMap1, vTexCoords[0][i] ).r;
		float D2 = tex2Dproj( shadowMap2, vTexCoords[1][i] ).r;
		float D3 = tex2Dproj( shadowMap3, vTexCoords[2][i] ).r;

		fShadowTerms[i] = ((D1 < S1) || (D2 < S2) || (D3 < S3)) ? 0.1f : 1.f;
	  	fShadowTerm += fShadowTerms[i];
   	}

	fShadowTerm /= 9.0f;

	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.Normal);

	float4 color  = g_light.ambient * g_material.ambient
			+ g_light.diffuse * g_material.diffuse * 0.2
			+ g_light.diffuse * g_material.diffuse * max(0, dot(N,L)) * fShadowTerm
			+ g_light.specular * pow( max(0, dot(N,H)), g_shininess);

	float4 Out = color * tex2D(colorMap, In.Tex);
	return Out;
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


		CullMode = CCW;
		//FillMode = solid;
       	 	ZEnable = true;
		AlphaBlendEnable = false;
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


		CullMode = CCW;
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

	        AlphaBlendEnable = true;
	        BlendOp = Add;
	        SrcBlend = SrcAlpha;
	        DestBlend = InvSrcAlpha;
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

	        AlphaBlendEnable = true;
	        BlendOp = Add;
	        SrcBlend = SrcAlpha;
	        DestBlend = InvSrcAlpha;
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


