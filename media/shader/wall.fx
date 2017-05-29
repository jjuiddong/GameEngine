//
// 2017-02-21, jjuiddong
// Wall Model Shader
//


float4x4 g_mView;
float4x4 g_mProj;
float4x4 g_mWorld;
float4x4 g_mWIT;
float4x4 g_mVPT; // ShadowMap Transform, = light view x light proj x uv transform
float4x4 g_mLVP; // ShadowMap Transform, Light View Projection, = light view x light proj
float4x4 g_mLightView;
float4x4 g_mLightProj;
float4x4 g_mLightTT;
float3 g_vEyePos;
float g_shininess = 90;
float g_fFarClip = 10000;
float g_uvFactor = 1.f;
float4 g_vFog= {1, 7000, 0, 0};
float4 g_fogColor = {0.2588f, 0.2941f, 0.4745f, 1}; // RGB(150,150,150)
float4x3 g_mPalette[ 64];

#define SHADOW_EPSILON 0.01f

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
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture g_shadowMapTexture;
sampler shadowMap = sampler_state
{
    	Texture = <g_shadowMapTexture>;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Border;
	AddressV = Border;

	BorderColor = 0xffffffff;
};



struct VS_AMBIENT_OUTPUT
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
	float3 Eye : TEXCOORD1;
};


struct VS_SCENE_OUTPUT
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
	float2 Depth : TEXCOORD4;
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

VS_AMBIENT_OUTPUT VS_Ambient(
	float4 Pos : POSITION,
	float3 Normal : NORMAL
)
{
	VS_AMBIENT_OUTPUT Out = (VS_AMBIENT_OUTPUT)0;
    
	float4x4 mVP = mul(g_mView, g_mProj);
	float3 worldPos = mul(Pos, g_mWorld).xyz;
	float3 N = normalize( mul(Normal, g_mWIT) );

	Out.Pos = mul( float4(worldPos,1), mVP );
	Out.Normal = N;
	Out.Eye = g_vEyePos - mul(Pos, g_mWorld).xyz;

	return Out;
}


float4 PS_Ambient(VS_AMBIENT_OUTPUT In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.Normal);

	float4 color = 	g_light.ambient * g_material.ambient
			+ (g_light.diffuse * g_material.diffuse * max(0, dot(N,L))) * 0.1f;

	float distance = length(In.Eye);
	float l = saturate((distance - g_vFog.x) / (g_vFog.y - g_vFog.x));
	color = lerp(color, g_fogColor * 0.5, l);

	return color;
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
// Scene

VS_SCENE_OUTPUT VS_Scene(
	float4 Pos : POSITION,
	float3 Normal : NORMAL
)
{
	VS_SCENE_OUTPUT Out = (VS_SCENE_OUTPUT)0;
    
	float4x4 mVP = mul(g_mView, g_mProj);
	float3 worldPos = mul(Pos, g_mWorld).xyz;
	float3 N = normalize( mul(Normal, g_mWIT) );

	Out.Pos = mul( float4(worldPos,1), mVP );
	Out.Normal = N;
	Out.Eye = g_vEyePos - mul(Pos, g_mWorld).xyz;

	return Out;
}


float4 PS_Scene(VS_SCENE_OUTPUT In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.Normal);


	float4 color  = g_light.diffuse * g_material.diffuse * max(0, dot(N,L))
			+ g_light.specular * pow( max(0, dot(N,H)), g_shininess);

	float distance = length(In.Eye);
	float l = saturate((distance - g_vFog.x) / (g_vFog.y - g_vFog.x));
	color = lerp(color, g_fogColor*0.5, l);

	return color;
}


float4 PS_Scene_NoShadow(VS_SCENE_OUTPUT In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.Normal);


	float4 color  = g_light.ambient * g_material.ambient
			+ g_light.diffuse * g_material.diffuse * max(0, dot(N,L))
			+ g_light.specular * pow( max(0, dot(N,H)), g_shininess);

	float distance = length(In.Eye);
	float l = saturate((distance - g_vFog.x) / (g_vFog.y - g_vFog.x));
	color = lerp(color, g_fogColor*0.5, l);

	return color;
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
	float3 N = normalize( mul(Normal, (float3x3)g_mWorld) );
	float4 wPos = mul(Pos, g_mWorld);

	Out.Pos = mul( Pos, mWVP );
	Out.Normal = N;
	Out.Eye = g_vEyePos - wPos.xyz;
	Out.Tex = Tex * g_uvFactor;
	//Out.TexShadow = mul( wPos, g_mVPT );
	//Out.vPos = mul( Pos, mWV);
	//Out.Depth = mul( wPos, g_mLVP ).zw;

	float4x4 mLVP = mul(g_mLightView, g_mLightProj);
	float4x4 mVPT = mul( mLVP, g_mLightTT);
	Out.TexShadow = mul( wPos, mVPT );
	Out.Depth = mul( wPos, mLVP ).zw;

	return Out;
}


float4 PS_Scene_ShadowMap(VS_OUTPUT_SHADOW In) : COLOR
{
	float4 vTexCoords[9];
	float fTexelSize = 1.0f / 1024.0f;
	float depth = min(In.Depth.x / In.Depth.y, 1.0);

	// Generate the tecture co-ordinates for the specified depth-map size
	// 4 3 5
	// 1 0 2
   	// 7 6 8
   	vTexCoords[0] = In.TexShadow;
   	vTexCoords[1] = In.TexShadow + float4( -fTexelSize, 0.0f, 0.0f, 0.0f );
   	vTexCoords[2] = In.TexShadow + float4(  fTexelSize, 0.0f, 0.0f, 0.0f );
   	vTexCoords[3] = In.TexShadow + float4( 0.0f, -fTexelSize, 0.0f, 0.0f );
   	vTexCoords[6] = In.TexShadow + float4( 0.0f,  fTexelSize, 0.0f, 0.0f );
   	vTexCoords[4] = In.TexShadow + float4( -fTexelSize, -fTexelSize, 0.0f, 0.0f );
   	vTexCoords[5] = In.TexShadow + float4(  fTexelSize, -fTexelSize, 0.0f, 0.0f );
   	vTexCoords[7] = In.TexShadow + float4( -fTexelSize,  fTexelSize, 0.0f, 0.0f );
   	vTexCoords[8] = In.TexShadow + float4(  fTexelSize,  fTexelSize, 0.0f, 0.0f );

        float fShadowTerms[9];
	float fShadowTerm = 0.0f;
   	for( int i = 0; i < 9; i++ )
   	{
	  	float A = tex2Dproj( shadowMap, vTexCoords[i] ).r;
	  	float B = (depth - SHADOW_EPSILON);

	  	fShadowTerms[i] = A < B ? 0.1f : 1.0f;
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
	//return float4(fShadowTerm, fShadowTerm, fShadowTerm, 1);
	//return fShadowTerms[0];
	//return fShadowTerm;
	//return tex2Dproj( shadowMap, vTexCoords[0] ).r;
	//return (tex2Dproj( shadowMap, vTexCoords[3] ).r - (depth - SHADOW_EPSILON)) * 1000;
	return Out;
}



//-----------------------------------------------------------------------------------
// Unlit

float4 PS_Unlit(VS_SCENE_OUTPUT In) : COLOR
{

	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.Normal);


	float4 color  = g_light.ambient * g_material.ambient
			+ g_light.diffuse * g_material.diffuse * (max(0, dot(N,L)) + 0.3)
			+ g_light.specular * pow( max(0, dot(N,H)), g_shininess);

	return color;
}




technique Ambient
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS_Ambient();
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
		VertexShader = compile vs_3_0 VS_Scene();
		PixelShader  = compile ps_3_0 PS_Scene();


		CullMode = CCW;
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
		VertexShader = compile vs_3_0 VS_Scene();
		PixelShader  = compile ps_3_0 PS_Scene_NoShadow();

        	ZEnable = true;
	        StencilEnable = false;
	        AlphaBlendEnable = false;
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
		FillMode = wireframe;
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
		VertexShader = compile vs_3_0 VS_Scene();
		PixelShader  = compile ps_3_0 PS_Unlit();

		CullMode = CCW;
		//FillMode = solid;
        	ZEnable = true;
	        ZFunc = LessEqual;
	        StencilEnable = false;
	        AlphaBlendEnable = false;
	        BlendOp = Add;
	        SrcBlend = One;
	        DestBlend = One;
	        StencilRef = 1;
	        StencilFunc = Greater;
	        StencilPass = Keep;
	}
}

