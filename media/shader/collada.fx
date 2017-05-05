
float4x4 g_mWorld;
float4x4 g_mView;
float4x4 g_mProj;
float4x4 g_mWIT;
float3 g_vEyePos;
float g_shininess = 90;

float4x3 g_mPalette[64];

struct Light
{
	float3 dir;		// world space direction
	float3 pos;		// world space position
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
};


texture g_normalMapTexture;
sampler2D normalMap = sampler_state
{
    Texture = <g_normalMapTexture>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};


texture g_specularMapTexture;
sampler2D specularMap = sampler_state
{
    Texture = <g_specularMapTexture>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};


texture g_selfIllumMapTexture;
sampler2D selfIllumMap = sampler_state
{
    Texture = <g_selfIllumMapTexture>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};


struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float4 Diffuse : COLOR0;
	float2 Tex : TEXCOORD0;
	float3 Eye : TEXCOORD1;
	float3 N : TEXCOORD2;
};


struct VS_SHADOW_OUTPUT
{
	float4 Pos : POSITION;
	float4 Diffuse : COLOR0;
};


struct VS_BUMP_OUTPUT
{
    float4 Pos	 : POSITION;
	float2 Tex : TEXCOORD0;
	float4 Diffuse : COLOR0;
	float4 Specular : COLOR1;
	float3 HalfVector : TEXCOORD1;
	float3 LightDir : TEXCOORD2;
};



// -------------------------------------------------------------
VS_OUTPUT VS_Skinning(
	float4 Pos : POSITION,
	float3 Normal : NORMAL,
	float2 Tex : TEXCOORD0,
	float4 Weights : TEXCOORD1,
	float4 BoneIndices : TEXCOORD2 // 본 인덱스 (4개 저장)
)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
    
	float4x4 mVP = mul(g_mView, g_mProj);
	float4x4 mWVP = mul(g_mWorld, mVP);

	float3 p = {0,0,0};
	float3 n = {0,0,0};

	p += mul(Pos, g_mPalette[ BoneIndices.x]) * Weights.x;
	p += mul(Pos, g_mPalette[ BoneIndices.y]) * Weights.y;
	p += mul(Pos, g_mPalette[ BoneIndices.z]) * Weights.z;
	p += mul(Pos, g_mPalette[ BoneIndices.w]) * Weights.w;

	n += mul(Normal, g_mPalette[ BoneIndices.x]) * Weights.x;
	n += mul(Normal, g_mPalette[ BoneIndices.y]) * Weights.y;
	n += mul(Normal, g_mPalette[ BoneIndices.z]) * Weights.z;
	n += mul(Normal, g_mPalette[ BoneIndices.w]) * Weights.w;

	Out.Pos = mul( float4(p,1), mWVP );
	n = normalize(n);

	float3 N = normalize( mul(n, (float3x3)g_mWorld) );
	
	Out.N = N;
	Out.Eye = g_vEyePos - mul(Pos, g_mWorld).xyz;
	Out.Tex = Tex;
    
    return Out;
}


float4 PS_Skinning(VS_OUTPUT In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 Out = g_light.ambient * g_material.ambient
		+ g_light.diffuse * g_material.diffuse * max(0, dot(N,L))
		+ g_light.specular * pow( max(0, dot(N,H)), g_shininess);

	Out = Out * tex2D(colorMap, In.Tex);
	return Out;
}




// -------------------------------------------------------------
VS_OUTPUT VS_Rigid(
	float4 Pos : POSITION,
	float3 Normal : NORMAL,
	float2 Tex : TEXCOORD0
)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

   	float4x4 mVP = mul(g_mView, g_mProj);
	float4 worldPos = mul(Pos, g_mWorld);
	float3 N = normalize( mul(Normal, (float3x3)g_mWorld) );

	Out.Pos = mul(worldPos, mVP);
	Out.N = N;
	Out.Eye = g_vEyePos - worldPos.xyz;
	Out.Tex = Tex;
   
	return Out;
}


VS_OUTPUT VS_Rigid_Ambient(
	float4 Pos : POSITION,
	float3 Normal : NORMAL,
	float2 Tex : TEXCOORD0
)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

   	float4x4 mVP = mul(g_mView, g_mProj);
	float4 worldPos = mul(Pos, g_mWorld);
	float3 N = normalize( mul(Normal, (float3x3)g_mWorld) );

	Out.Pos = mul(worldPos, mVP);
	Out.N = N;
	Out.Eye = g_vEyePos - worldPos.xyz;
	Out.Tex = Tex;
   
	return Out;
}


// -------------------------------------------------------------
// 2패스:정점셰이더, 그림자 맵 출력.
// -------------------------------------------------------------
VS_SHADOW_OUTPUT VS_pass1(
	float4 Pos : POSITION,
	float3 Normal : NORMAL,
	float2 Tex : TEXCOORD0,
	float4 Weights : TEXCOORD1,
	float4 BoneIndices : TEXCOORD2 // 본 인덱스 (4개 저장)
)
{
	VS_SHADOW_OUTPUT Out = (VS_SHADOW_OUTPUT)0; // 출력데이터
    
	// 좌표변환
	float4x4 mVP = mul(g_mView, g_mProj);
	float4x4 mWVP = mul(g_mWorld, mVP);

	float3 p = {0,0,0};

	p += mul(Pos, g_mPalette[ BoneIndices.x]) * Weights.x;
	p += mul(Pos, g_mPalette[ BoneIndices.y]) * Weights.y;
	p += mul(Pos, g_mPalette[ BoneIndices.z]) * Weights.z;
	p += mul(Pos, g_mPalette[ BoneIndices.w]) * Weights.w;

	Out.Pos = mul( float4(p,1), mWVP );
	Out.Diffuse = float4(1,1,1,1);
    
    return Out;
}



// -------------------------------------------------------------
// 4패스: 스키닝 + 범프 매핑 정점 셰이더
// D3D9NormalMapping 소스를 참조 함.
// http://www.dhpoware.com/demos/d3d9NormalMapping.html
// -------------------------------------------------------------
VS_BUMP_OUTPUT VS_pass4(
	float4 Pos : POSITION,          // 모델정점
	float3 Normal : NORMAL,		// 법선벡터
	float2 Tex : TEXCOORD0,		// 텍스쳐 좌표
	float4 Weights : TEXCOORD1,	// 버텍스 가중치
	float4 BoneIndices : TEXCOORD2, // 본 인덱스 (4개 저장)
	float3 tangent : TANGENT,		// tangent 벡터
	float3 binormal : BINORMAL 	// binormal 벡터
)
{
	VS_BUMP_OUTPUT Out = (VS_BUMP_OUTPUT)0; // 출력데이터
    
	float3 p = {0,0,0};
	float3 n = {0,0,0};

	p += mul(Pos, g_mPalette[ BoneIndices.x]).xyz * Weights.x;
	p += mul(Pos, g_mPalette[ BoneIndices.y]).xyz * Weights.y;
	p += mul(Pos, g_mPalette[ BoneIndices.z]).xyz * Weights.z;
	p += mul(Pos, g_mPalette[ BoneIndices.w]).xyz * Weights.w;

	n += mul(float4(Normal,0), g_mPalette[ BoneIndices.x]).xyz * Weights.x;
	n += mul(float4(Normal,0), g_mPalette[ BoneIndices.y]).xyz * Weights.y;
	n += mul(float4(Normal,0), g_mPalette[ BoneIndices.z]).xyz * Weights.z;
	n += mul(float4(Normal,0), g_mPalette[ BoneIndices.w]).xyz * Weights.w;

	float3 worldPos = mul(float4(p,1), g_mWorld).xyz;
	float3 lightDir = -g_light.dir;
	float3 viewDir = g_vEyePos - worldPos;
	float3 halfVector = normalize(normalize(lightDir) + normalize(viewDir));

	n = normalize( mul(n, (float3x3)g_mWorld) );
	float3 t = normalize( mul(tangent, (float3x3)g_mWorld) );
	float3 b = normalize( mul(binormal, (float3x3)g_mWorld) );
	float3x3 tbnMatrix = float3x3(t.x, b.x, n.x,
	                              t.y, b.y, n.y,
	                              t.z, b.z, n.z);

	float4x4 mVP = mul(g_mView, g_mProj);
	Out.Pos = mul( float4(worldPos,1), mVP );
	Out.Tex = Tex;
	Out.HalfVector = mul(halfVector, tbnMatrix);
	Out.LightDir = mul(lightDir, tbnMatrix);    
	Out.Diffuse = g_material.diffuse * g_light.diffuse;
	Out.Specular = g_material.specular * g_light.specular;

    return Out;
}


// -------------------------------------------------------------
// 4패스:픽셀셰이더
// -------------------------------------------------------------
float4 PS_pass4(VS_BUMP_OUTPUT In) : COLOR
{
	float3 n = normalize(tex2D(normalMap, In.Tex).rgb  * 2.0f - 1.0f);
    float3 h = normalize(In.HalfVector);
    float3 l = normalize(In.LightDir);

    float nDotL = saturate(dot(n, l));
    float nDotH = saturate(dot(n, h));
    float power = (nDotL == 0.0f) ? 0.0f : pow(nDotH, g_shininess);

    float4 color = (g_material.ambient * g_light.ambient)
						+ (In.Diffuse * nDotL) 
						+ (In.Specular * power);

	return  color * tex2D(colorMap, In.Tex);
}




technique Skinning
{
	pass P0
	{
	        VertexShader = compile vs_3_0 VS_Skinning();
		PixelShader  = compile ps_3_0 PS_Skinning();
	}
}


technique Rigid
{
	pass P0
	{
	        VertexShader = compile vs_3_0 VS_Rigid();
		PixelShader  = compile ps_3_0 PS_Skinning();
	}
}


technique Rigid_Ambient
{
	pass P0
	{
	        VertexShader = compile vs_3_0 VS_Rigid_Ambient();
		PixelShader  = compile ps_3_0 PS_Skinning_Ambient();

		CullMode = CCW;
		AlphaBlendEnable = false;
	}
}


technique Rigid_Scene
{
	pass P0
	{
	        VertexShader = compile vs_3_0 VS_Rigid();
		PixelShader  = compile ps_3_0 PS_Skinning();

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




technique TShader
{
    pass P0
    {
        // 기본 조명 스키닝 애니메이션
        VertexShader = compile vs_3_0 VS_Skinning();
	PixelShader  = compile ps_3_0 PS_Skinning();
    }

    pass P1
    {
        // 그림자 맵 셰이더
        VertexShader = compile vs_3_0 VS_pass1();
    }

    pass P2
    {
	// 아무것도 없음.
    }

    pass P3
    {
	// 아무것도 없음.
    }

    pass P4
    {
	// 스키닝 애니메이션 + 범프 맵.
	VertexShader = compile vs_3_0 VS_pass4();
	PixelShader  = compile ps_3_0 PS_pass4();
    }

}

