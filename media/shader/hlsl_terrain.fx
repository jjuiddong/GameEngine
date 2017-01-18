
// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4 mWorld;
float4x4 mVP;		// ���ÿ��� �������������� ��ǥ��ȯ
float4x4 mWIT;
float4x4 mWVPT;
float3 vEyePos; // ī�޶� ��ġ.
float4 vFog;
float4 fogColor = {0.58823f, 0.58823f, 0.58823f, 1}; // RGB(150,150,150)


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



// ------------------------------------------------------------
// �ؽ�ó
// ------------------------------------------------------------
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


// ------------------------------------------------------------
// �׸��ڸ�
// ------------------------------------------------------------
texture ShadowMap;
sampler ShadowMapSamp = sampler_state
{
    Texture = <ShadowMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

	AddressU = Clamp;
	AddressV = Clamp;
};


struct VS_OUTPUT
{
    float4 Pos	 : POSITION;
	float4 Diffuse : COLOR0;
	float2 Tex : TEXCOORD0;
	float3 Eye : TEXCOORD1;
	float3 N : TEXCOORD2;
};


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
// 0�н�: �������̴�, �� + ����
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
      float4 Pos : POSITION,          // ������
	  float3 Normal : NORMAL,		// ��������
	  float2 Tex : TEXCOORD0
)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����

	float4 worldPos = mul( Pos, mWorld );
	float3 N = normalize( mul(Normal, (float3x3)mWIT) ); // ���� ��ǥ�迡���� ����.

	Out.Pos = mul(worldPos, mVP );
	Out.N = N;
	Out.Eye = vEyePos - worldPos.xyz;
	Out.Tex = Tex;
    
    return Out;
}


// -------------------------------------------------------------
// 0�н�: �ȼ����̴�, �� + ����.
// -------------------------------------------------------------
float4 PS_pass0(VS_OUTPUT In) : COLOR
{
	float3 L = -light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = light.ambient * material.ambient
				+ light.diffuse * material.diffuse * max(0, dot(N,L));
				+ light.specular * pow( max(0, dot(N,H)), 16);

	float4 Out = color * tex2D(colorMap, In.Tex);
    return Out;
}


// -------------------------------------------------------------
// 1�н�: �������̴�, ���� ���
// -------------------------------------------------------------
VS_OUTPUT VS_pass1(
      float4 Pos : POSITION,          // ������
	  float3 Normal : NORMAL,		// ��������
	  float2 Tex : TEXCOORD0
)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
	float4 worldPos = mul( Pos, mWorld );
	float3 N = normalize( mul(Normal, (float3x3)mWIT) ); // ���� ��ǥ�迡���� ����.

	Out.Pos = mul( worldPos, mVP );
	Out.N = N;
	Out.Eye = vEyePos - worldPos.xyz;
	Out.Tex = Tex;
    
    return Out;
}

// -------------------------------------------------------------
// 1�н�: �ȼ����̴�, �� + ���� + ���� ���.
// -------------------------------------------------------------
float4 PS_pass1(VS_OUTPUT In) : COLOR
{
	float3 L = -light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = light.ambient * material.ambient
				+ light.diffuse * material.diffuse * max(0, dot(N,L));
				+ light.specular * pow( max(0, dot(N,H)), 16);

	float4 Out = color * tex2D(colorMap, In.Tex);

	float distance = length(In.Eye);
	float l = saturate((distance-vFog.x) / (vFog.y - vFog.x));
	Out = lerp(Out, fogColor, l);

    return Out;
}



// -------------------------------------------------------------
// 2�н�:�������̴�, �� + ���� + ���� + �׸��� ���.
// -------------------------------------------------------------
VS_OUTPUT_SHADOW VS_pass2(
      float4 Pos : POSITION,          // ������
	  float3 Normal : NORMAL,		// ��������
	  float2 Tex : TEXCOORD0
)
{
    VS_OUTPUT_SHADOW Out = (VS_OUTPUT_SHADOW)0;        // ��µ�����
    
	float4 worldPos = mul( Pos, mWorld );
	float3 N = normalize( mul(Normal, (float3x3)mWIT) ); // ���� ��ǥ�迡���� ����.

	Out.Pos = mul( worldPos, mVP );
	Out.N = N;
	Out.Eye = vEyePos - worldPos.xyz;
	Out.Tex = Tex;
	Out.TexShadow = mul( Pos, mWVPT );
    
    return Out;
}

// -------------------------------------------------------------
// 2�н�:�ȼ����̴�, �� + ���� + �׸��� + ���� ���.
// -------------------------------------------------------------
float4 PS_pass2(VS_OUTPUT_SHADOW In) : COLOR
{
	float3 L = -light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = 	light.ambient * material.ambient
				+ light.diffuse * material.diffuse * max(0, dot(N,L));
				+ light.specular * pow( max(0, dot(N,H)), 16);

	float4 Out = color * tex2D(colorMap, In.Tex);

	float4 shadow = tex2Dproj( ShadowMapSamp, In.TexShadow );
	Out = Out * saturate(color - (0.8f*shadow));

	float distance = length(In.Eye);
	float l = saturate((distance-vFog.x) / (vFog.y - vFog.x));
	Out = lerp(Out, fogColor, l);

    return Out;
}


	
// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
technique TShader
{

	// �� ���̵�
    pass P0
    {
        VertexShader = compile vs_3_0 VS_pass0();
		PixelShader  = compile ps_3_0 PS_pass0();
    }


	// �� + ���� + ���� ���̵�.
    pass P1
    {
        VertexShader = compile vs_3_0 VS_pass1();
		PixelShader  = compile ps_3_0 PS_pass1();
    }


    // �� + ���� + �׸���  + ���� ���
    pass P2
    {
        VertexShader = compile vs_3_0 VS_pass2();
		PixelShader  = compile ps_3_0 PS_pass2();
    }


}
