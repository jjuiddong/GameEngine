
// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4 g_mWorld;
float4x4 g_mVP;
float4x4 g_mView;
float4x4 g_mProj;
//float4x4 g_mWIT;
float4x4 g_mWVPT;
float3 g_vEyePos;
float4 g_vFog;
float4 g_fogColor = {0.58823f, 0.58823f, 0.58823f, 1}; // RGB(150,150,150)
float g_alphaUVFactor = 8.f;


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



// ------------------------------------------------------------
// �ؽ�ó
// ------------------------------------------------------------
texture g_colorMapTexture;
sampler colorMap = sampler_state
{
    Texture = <g_colorMapTexture>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;

    AddressU = Wrap;
    AddressV = Wrap;
};


// ------------------------------------------------------------
// �׸��ڸ�
// ------------------------------------------------------------
texture g_ShadowMap;
sampler ShadowMapSamp = sampler_state
{
    Texture = <g_ShadowMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;

	AddressU = Border;
	AddressV = Border;
};


// ------------------------------------------------------------
// ���÷��� ���� �ؽ���
// ------------------------------------------------------------
texture g_SplattingAlphaMap;
sampler SplattingMapSamp = sampler_state
{
    Texture = <g_SplattingAlphaMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;

	AddressU = Border;
	AddressV = Border;
};


// ------------------------------------------------------------
// ���÷��� �ؽ�ó ���̾�1
// ------------------------------------------------------------
texture Tex1;
sampler Samp1 = sampler_state
{
    Texture = <Tex1>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};

// ------------------------------------------------------------
// ���÷��� �ؽ�ó ���̾�2
// ------------------------------------------------------------
texture Tex2;
sampler Samp2 = sampler_state
{
    Texture = <Tex2>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};

// ------------------------------------------------------------
// ���÷��� �ؽ�ó ���̾�3
// ------------------------------------------------------------
texture Tex3;
sampler Samp3 = sampler_state
{
    Texture = <Tex3>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};


// ------------------------------------------------------------
// ���÷��� �ؽ�ó ���̾�4
// ------------------------------------------------------------
texture Tex4;
sampler Samp4 = sampler_state
{
    Texture = <Tex4>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};


struct VS_OUTPUT
{
    float4 Pos	 : POSITION;
	float4 Diffuse : COLOR0;
	float2 Tex : TEXCOORD0;
	float3 Eye : TEXCOORD1;
	float3 N : TEXCOORD2;
};


// �� + �׸���.
struct VS_OUTPUT_SHADOW
{
    float4 Pos	 : POSITION;
	float4 Diffuse : COLOR0;
	float2 Tex : TEXCOORD0;
	float4 TexShadow : TEXCOORD1;
	float3 Eye : TEXCOORD2;
	float3 N : TEXCOORD3;
};



// -------------------------------------------------------------
// 0�н�: ���� + ����
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
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
// 0�н�:�ȼ����̴�
// -------------------------------------------------------------
float4 PS_pass0(VS_OUTPUT In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = g_light.ambient * g_material.ambient
				+ g_light.diffuse * g_material.diffuse * max(0, dot(N,L));
				+g_light.specular * pow( max(0, dot(N,H)), 16);

	float4 Out = color * tex2D(colorMap, In.Tex);
    return Out;
}



// -------------------------------------------------------------
// 1�н�: ���� + ���� + ����
// -------------------------------------------------------------
VS_OUTPUT VS_pass1(
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
// 1�н�: �ȼ����̴�, ���� + ���� + ���� ���.
// -------------------------------------------------------------
float4 PS_pass1(VS_OUTPUT In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = g_light.ambient * g_material.ambient
				+ g_light.diffuse * g_material.diffuse * max(0, dot(N,L));
				+g_light.specular * pow( max(0, dot(N,H)), 16);

	float4 Out = color * tex2D(colorMap, In.Tex);

	float distance = length(In.Eye);
	float l = saturate((distance- g_vFog.x) / (g_vFog.y - g_vFog.x));
	Out = lerp(Out, g_fogColor, l);

    return Out;
}



// -------------------------------------------------------------
// 2�н�: ���� + ���� + �׸���.
// -------------------------------------------------------------
VS_OUTPUT_SHADOW VS_pass2(
		float4 Pos : POSITION,
		float3 Normal : NORMAL,
		float2 Tex : TEXCOORD0
)
{
	VS_OUTPUT_SHADOW Out = (VS_OUTPUT_SHADOW)0;
   
	float4x4 mVP = mul(g_mView, g_mProj);
	float4 worldPos = mul(Pos, g_mWorld);
	float3 N = normalize( mul(Normal, (float3x3)g_mWorld) );

	Out.Pos = mul(worldPos, mVP);
	Out.N = N;
	Out.Eye = g_vEyePos - worldPos.xyz;
	Out.Tex = Tex;
	Out.TexShadow = mul( Pos, g_mWVPT);
    
	return Out;
}


// -------------------------------------------------------------
// 2�н�:�ȼ����̴�,		���� + ���� + ���� + �׸���
// -------------------------------------------------------------
float4 PS_pass2(VS_OUTPUT_SHADOW In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = g_light.ambient * g_material.ambient
			+ g_light.diffuse * g_material.diffuse * max(0, dot(N,L))
			+ g_light.specular * pow( max(0, dot(N,H)), 16);

	//return color;

	float4 Out = color * tex2D(colorMap, In.Tex);

	float4 shadow = tex2Dproj( ShadowMapSamp, In.TexShadow );
	Out = Out * saturate(1.3f - shadow);

	float distance = length(In.Eye);
	float l = saturate((distance- g_vFog.x) / (g_vFog.y - g_vFog.x));
	Out = lerp(Out, g_fogColor, l);

	return Out;
}


// -------------------------------------------------------------
// 3�н�: ���� + ����
// -------------------------------------------------------------
VS_OUTPUT_SHADOW VS_pass3(
      float4 Pos : POSITION,
	  float3 Normal : NORMAL,
	  float2 Tex : TEXCOORD0
)
{
	VS_OUTPUT_SHADOW Out = (VS_OUTPUT_SHADOW)0;
    
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
// 3�н�:�ȼ����̴�,		���� + ���� + ���� + ���÷��� ���
// -------------------------------------------------------------
float4 PS_pass3(VS_OUTPUT_SHADOW In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = g_light.ambient * g_material.ambient
			+ g_light.diffuse * g_material.diffuse * max(0, dot(N,L))
			+g_light.specular * pow( max(0, dot(N,H)), 16);

	float4 Out = color * tex2D(colorMap, In.Tex);

	float4 alpha = tex2D(SplattingMapSamp, (In.Tex / g_alphaUVFactor));
	Out = (alpha.a * (color * tex2D(Samp1, In.Tex))) + ((1 - alpha.a) * Out);
	Out = (alpha.r * (color * tex2D(Samp2, In.Tex))) + ((1 - alpha.r) * Out);
	Out = (alpha.g * (color * tex2D(Samp3, In.Tex))) + ((1 - alpha.g) * Out);
	Out = (alpha.b * (color * tex2D(Samp4, In.Tex))) + ((1 - alpha.b) * Out);

	float distance = length(In.Eye);
	float l = saturate((distance- g_vFog.x) / (g_vFog.y - g_vFog.x));
	Out = lerp(Out, g_fogColor, l);

	return Out;
}



// -------------------------------------------------------------
// 4�н�:�ȼ����̴�,		���� + ���÷���  (����X, ����X)
// -------------------------------------------------------------
float4 PS_pass4(VS_OUTPUT In) : COLOR
{
	float4 Out = tex2D(colorMap, In.Tex);

	float4 alpha = tex2D(SplattingMapSamp, (In.Tex / g_alphaUVFactor));
	Out = (alpha.a * (tex2D(Samp1, In.Tex))) + ((1 - alpha.a) * Out);
	Out = (alpha.r * (tex2D(Samp2, In.Tex))) + ((1 - alpha.r) * Out);
	Out = (alpha.g * (tex2D(Samp3, In.Tex))) + ((1 - alpha.g) * Out);
	Out = (alpha.b * (tex2D(Samp4, In.Tex))) + ((1 - alpha.b) * Out);

    return Out;
}



// -------------------------------------------------------------
// 5�н�:�ȼ����̴�,		���� + ���� + ���� + ���÷��� ��� + �׸���.
// -------------------------------------------------------------
float4 PS_pass5(VS_OUTPUT_SHADOW In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = g_light.ambient * g_material.ambient
				+ g_light.diffuse * g_material.diffuse * max(0, dot(N,L));
				+g_light.specular * pow( max(0, dot(N,H)), 16);

	float4 Out = color * tex2D(colorMap, In.Tex);

	float4 alpha = tex2D(SplattingMapSamp, (In.Tex / g_alphaUVFactor));
	Out = (alpha.a * (color * tex2D(Samp1, In.Tex))) + ((1 - alpha.a) * Out);
	Out = (alpha.r * (color * tex2D(Samp2, In.Tex))) + ((1 - alpha.r) * Out);
	Out = (alpha.g * (color * tex2D(Samp3, In.Tex))) + ((1 - alpha.g) * Out);
	Out = (alpha.b * (color * tex2D(Samp4, In.Tex))) + ((1 - alpha.b) * Out);

	float4 shadow = tex2Dproj( ShadowMapSamp, In.TexShadow );
	Out = Out * saturate(color - (0.8f*shadow));

	float distance = length(In.Eye);
	float l = saturate((distance- g_vFog.x) / (g_vFog.y - g_vFog.x));
	Out = lerp(Out, g_fogColor, l);

    return Out;
}


	
// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
technique TShader
{

	// ���� + ����
    pass P0
    {
        VertexShader = compile vs_3_0 VS_pass0();
	PixelShader  = compile ps_3_0 PS_pass0();
    }


	// ���� + ���� + ����
    pass P1
    {
        VertexShader = compile vs_3_0 VS_pass1();
	PixelShader  = compile ps_3_0 PS_pass1();
    }


    // ���� + ���� + �׸���
    pass P2
    {
        VertexShader = compile vs_3_0 VS_pass2();
	PixelShader  = compile ps_3_0 PS_pass2();
    }


	// ���� + ���� + ���� + ���÷���
    pass P3
    {
        VertexShader = compile vs_3_0 VS_pass3();
	PixelShader  = compile ps_3_0 PS_pass3();
    }


	// ���� + ���÷��� (����X, ����X) (���� �ؽ��� �����)
    pass P4
    {
	VertexShader = compile vs_3_0 VS_pass0();
	PixelShader  = compile ps_3_0 PS_pass4();
    }


	// ���� + ���� + ���� + ���÷��� + �׸���
    pass P5
    {
        VertexShader = compile vs_3_0 VS_pass2();
	PixelShader  = compile ps_3_0 PS_pass5();
    }
}

