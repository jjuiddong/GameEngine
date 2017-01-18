// ���� ������Ʈ�� ����ϱ� ���� ���̴� �ڵ�.

// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4 mWorld;
float4x4 mVP;		// ���ÿ��� �������������� ��ǥ��ȯ
float4x4 mWIT;
float4x4 mWVPT;
float3 vEyePos;
float4 vFog;
float4 fogColor = {0.58823f, 0.58823f, 0.58823f, 1}; // RGB(150,150,150)
float shininess = 90;
float4 globalAmbient = {0.2f, 0.2f, 0.2f, 1.0f};


// �� ���̴������� �������� ������� ������, �ڵ��� �Ϲ�ȭ�� ���ؼ�
// ������ ���� �Ǿ���. 
// light, material ������ ����Ǿ����� �������� �ʴ´�.
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


// ------------------------------------------------------------
// ��ָ�
// ------------------------------------------------------------
texture normalMapTexture;
sampler2D normalMap = sampler_state
{
    Texture = <normalMapTexture>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};


struct VS_OUTPUT
{
    float4 Pos	 : POSITION;
	float4 Diffuse : COLOR0;
	float2 Tex : TEXCOORD0;
};

struct VS_SHADOW_OUTPUT
{
	float4 Pos : POSITION;
	float4 Diffuse : COLOR0;
};

struct VS_FOG_OUTPUT
{
    float4 Pos	 : POSITION;
	float4 Diffuse : COLOR0;
	float2 Tex : TEXCOORD0;
	float3 Eye : TEXCOORD1;
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
// 0�н�: �������̴�
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
      float4 Pos : POSITION, // ������
	  float2 Tex : TEXCOORD0
)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����

	float4x4 mWVP = mul(mWorld, mVP);
	Out.Pos = mul( Pos, mWVP );
	Out.Tex = Tex;

    return Out;
}


// -------------------------------------------------------------
// 0�н�: �ȼ����̴�
// -------------------------------------------------------------
float4 PS_pass0(VS_OUTPUT In) : COLOR
{
	return tex2D(colorMap, In.Tex);
}



// -------------------------------------------------------------
// 1�н�: �׸��� �� ����.
// -------------------------------------------------------------
VS_SHADOW_OUTPUT VS_pass1(
      float4 Pos : POSITION,          // ������
	  float3 Normal : NORMAL,		// ��������
	  float2 Tex : TEXCOORD0
)
{
    VS_SHADOW_OUTPUT Out = (VS_SHADOW_OUTPUT)0;  // ��µ�����

	float4x4 mWVP = mul(mWorld, mVP);
	Out.Pos = mul( Pos, mWVP );
	Out.Diffuse = float4(1,1,1,1);

    return Out;
}



// -------------------------------------------------------------
// 2�н�: ���� ���
// -------------------------------------------------------------
VS_FOG_OUTPUT VS_pass2(
      float4 Pos : POSITION,          // ������
	  float3 Normal : NORMAL,		// ��������
	  float2 Tex : TEXCOORD0
)
{
    VS_FOG_OUTPUT Out = (VS_FOG_OUTPUT)0;        // ��µ�����
    
	float4 worldPos = mul( Pos, mWorld );

	Out.Pos = mul(worldPos, mVP);
	Out.Tex = Tex;
	Out.Eye = vEyePos - worldPos.xyz;
    
    return Out;
}

// -------------------------------------------------------------
// 2�н�: ���� ���.
// -------------------------------------------------------------
float4 PS_pass2(VS_FOG_OUTPUT In) : COLOR
{
	float4 Out;

	Out = tex2D(colorMap, In.Tex);

	float distance = length(In.Eye);
	float l = saturate((distance-vFog.x) / (vFog.y - vFog.x));
	Out = lerp(Out, fogColor, l);

    return Out;
}



// -------------------------------------------------------------
// 3�н�: ���� + �׸��� ���.
// -------------------------------------------------------------
VS_OUTPUT_SHADOW VS_pass3(
      float4 Pos : POSITION,          // ������
	  float3 Normal : NORMAL,		// ��������
	  float2 Tex : TEXCOORD0
)
{
    VS_OUTPUT_SHADOW Out = (VS_OUTPUT_SHADOW)0;        // ��µ�����
    
	float4x4 mWVP = mul(mWorld, mVP);
	Out.Pos = mul( Pos, mWVP );
	Out.Tex = Tex;
	Out.TexShadow = mul( Pos, mWVPT );
    
    return Out;
}


// -------------------------------------------------------------
// 3�н�: ���� + �׸��� ���.
// -------------------------------------------------------------
float4 PS_pass3(VS_OUTPUT_SHADOW In) : COLOR
{
	float4 color = tex2D(colorMap, In.Tex);
	float4 shadow = tex2Dproj( ShadowMapSamp, In.TexShadow );
	float4 Out = saturate(color - (0.8f*shadow));

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

	// �׸��� �� ���� ���̴�
	Pass P1
	{
		VertexShader = compile vs_3_0 VS_pass1();
	}


	// ���� ���̵�.
    pass P2
    {
        VertexShader = compile vs_3_0 VS_pass2();
		PixelShader  = compile ps_3_0 PS_pass2();
    }


    // �� + �׸��� ��� ���̴�
    pass P3
    {
        VertexShader = compile vs_3_0 VS_pass3();
		PixelShader  = compile ps_3_0 PS_pass3();
    }


	// �� + �������� -> �⺻ ���̴��� ���.
    pass P4
    {
        VertexShader = compile vs_3_0 VS_pass0();
		PixelShader  = compile ps_3_0 PS_pass0();
    }


}
