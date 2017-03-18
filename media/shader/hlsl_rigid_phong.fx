// ���� ������Ʈ�� ����ϱ� ���� ���̴� �ڵ�.

// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4 g_mWorld;
//float4x4 g_mVP;		// ���ÿ��� �������������� ��ǥ��ȯ
float4x4 g_mWVPT;
float4x4 g_mView;
float4x4 g_mProj;                   // Projection matrix
float3 g_vEyePos;
float4 g_vFog;
float4 g_fogColor = {0.58823f, 0.58823f, 0.58823f, 1}; // RGB(150,150,150)
float g_shininess = 90;
float4 g_globalAmbient = {0.2f, 0.2f, 0.2f, 1.0f};


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
    MipFilter = NONE;

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
    MipFilter = NONE;

	AddressU = Clamp;
	AddressV = Clamp;
};


// ------------------------------------------------------------
// ��ָ�
// ------------------------------------------------------------
texture g_normalMapTexture;
sampler2D normalMap = sampler_state
{
    Texture = <g_normalMapTexture>;
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
	float3 Eye : TEXCOORD1;
	float3 N : TEXCOORD2;
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


struct VS_SHADOW_OUTPUT
{
	float4 Pos : POSITION;
	float4 Diffuse : COLOR0;
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
// 0�н�:�������̴�
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
      float4 Pos : POSITION,          // ������
	  float3 Normal : NORMAL,		// ��������
	  float2 Tex : TEXCOORD0
)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
	float4x4 mVP = mul(g_mView, g_mProj);
	float4 worldPos = mul(Pos, g_mWorld);
	float3 N = normalize( mul(Normal, (float3x3)g_mWorld) ); // ���� ��ǥ�迡���� ����.

	Out.Pos = mul(worldPos, mVP);
	Out.N = N;
	Out.Eye = g_vEyePos - worldPos.xyz;
	Out.Tex = Tex;
    
    return Out;
}


// -------------------------------------------------------------
// 0�н�: �ȼ����̴�, �����̵�
// -------------------------------------------------------------
float4 PS_pass0(VS_OUTPUT In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = g_light.ambient * g_material.ambient
				+ g_light.diffuse * g_material.diffuse * max(0, dot(N,L));
				+ g_light.specular * pow( max(0, dot(N,H)), 16);

	float4 Out = color * tex2D(colorMap, In.Tex);
    return Out;
}



// -------------------------------------------------------------
// 1�н�:�������̴�, �׸��� �� ����.
// -------------------------------------------------------------
VS_SHADOW_OUTPUT VS_pass1(
      float4 Pos : POSITION,          // ������
	  float3 Normal : NORMAL,		// ��������
	  float2 Tex : TEXCOORD0
)
{
    VS_SHADOW_OUTPUT Out = (VS_SHADOW_OUTPUT)0;  // ��µ�����
    
	float4x4 mVP = mul(g_mView, g_mProj);
	float4x4 mWVP = mul(g_mWorld, mVP);
	Out.Pos = mul( Pos, mWVP );
	Out.Diffuse = float4(1,1,1,1);

    return Out;
}



// -------------------------------------------------------------
// 2�н�: �������̴�, ���� ���
// -------------------------------------------------------------
VS_OUTPUT VS_pass2(
      float4 Pos : POSITION,          // ������
	  float3 Normal : NORMAL,		// ��������
	  float2 Tex : TEXCOORD0
)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
	float4 worldPos = mul(Pos, g_mWorld);
	float3 N = normalize( mul(Normal, (float3x3)g_mWorld) ); // ���� ��ǥ�迡���� ����.

	float4x4 mVP = mul(g_mView, g_mProj);
	Out.Pos = mul(worldPos, mVP);
	Out.N = N;
	Out.Eye = g_vEyePos - worldPos.xyz;
	Out.Tex = Tex;
    
    return Out;
}


// -------------------------------------------------------------
// 2�н�: �ȼ����̴�, ���� ���.
// -------------------------------------------------------------
float4 PS_pass2(VS_OUTPUT In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = g_light.ambient * g_material.ambient
				+ g_light.diffuse * g_material.diffuse * max(0, dot(N,L));
				+g_light.specular * pow( max(0, dot(N,H)), 16);

	float4 Out = color * tex2D(colorMap, In.Tex);

	float distance = length(In.Eye);
	float l = saturate((distance-g_vFog.x) / (g_vFog.y - g_vFog.x));
	Out = lerp(Out, g_fogColor, l);

    return Out;
}



// -------------------------------------------------------------
// 3�н�: �������̴�, ���� + �׸��� ���.
// -------------------------------------------------------------
VS_OUTPUT_SHADOW VS_pass3(
      float4 Pos : POSITION,          // ������
	  float3 Normal : NORMAL,		// ��������
	  float2 Tex : TEXCOORD0
)
{
    VS_OUTPUT_SHADOW Out = (VS_OUTPUT_SHADOW)0;        // ��µ�����
    
	float4x4 mVP = mul(g_mView, g_mProj);
	float4 worldPos = mul(Pos, g_mWorld);
	float3 N = normalize( mul(Normal, (float3x3)g_mWorld) ); // ���� ��ǥ�迡���� ����.

	Out.Pos = mul(worldPos, mVP);
	Out.N = N;
	Out.Eye = g_vEyePos - worldPos.xyz;
	Out.Tex = Tex;
	Out.TexShadow = mul( Pos, g_mWVPT );
    
    return Out;
}

// -------------------------------------------------------------
// 3�н�:�ȼ����̴�, ���� + �׸��� ���.
// -------------------------------------------------------------
float4 PS_pass3(VS_OUTPUT_SHADOW In) : COLOR
{
	float3 L = -g_light.dir;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	float4 color = g_light.ambient * g_material.ambient
				+ g_light.diffuse * g_material.diffuse * max(0, dot(N,L));
				+g_light.specular * pow( max(0, dot(N,H)), 16);

	float4 Out = color * tex2D(colorMap, In.Tex);

	float4 shadow = tex2Dproj( ShadowMapSamp, In.TexShadow );
	Out = Out * saturate(color - (0.8f*shadow));

	float distance = length(In.Eye);
	float l = saturate((distance- g_vFog.x) / (g_vFog.y - g_vFog.x));
	Out = lerp(Out, g_fogColor, l);

    return Out;
}



// -------------------------------------------------------------
// 4�н�: �������̴�, ���� ����
// D3D9NormalMapping �ҽ��� ���� ��.
// http://www.dhpoware.com/demos/d3d9NormalMapping.html
// -------------------------------------------------------------
VS_BUMP_OUTPUT VS_pass4(
      float4 Pos : POSITION,          // ������
	  float3 Normal : NORMAL,		// ��������
	  float2 Tex : TEXCOORD0,		// �ؽ��� ��ǥ
	  float3 tangent : TANGENT	,	// tangent ����
	  float3 binormal : BINORMAL 	// binormal ����
)
{
    VS_BUMP_OUTPUT Out = (VS_BUMP_OUTPUT)0;

	float3 worldPos = mul(Pos, g_mWorld).xyz;
	float3 lightDir = -g_light.dir;
	float3 viewDir = g_vEyePos - worldPos;
	float3 halfVector = normalize(normalize(lightDir) + normalize(viewDir));
	
	float3 n = normalize( mul(Normal, (float3x3)g_mWorld) ); // ���� ��ǥ�迡���� ����.
	float3 t = normalize( mul(tangent, (float3x3)g_mWorld) ); // ���� ��ǥ�迡���� ź��Ʈ
	float3 b = normalize( mul(binormal, (float3x3)g_mWorld) ); // ���� ��ǥ�迡���� ���̳��
	float3x3 tbnMatrix = float3x3(t.x, b.x, n.x,
	                              t.y, b.y, n.y,
	                              t.z, b.z, n.z);
    
	float4x4 mVP = mul(g_mView, g_mProj);
	Out.Pos = mul(float4(worldPos,1), mVP);
	Out.Tex = Tex;
	Out.HalfVector = mul(halfVector, tbnMatrix);
	Out.LightDir = mul(lightDir, tbnMatrix);
	Out.Diffuse = g_material.diffuse * g_light.diffuse;
	Out.Specular = g_material.specular * g_light.specular;
    
    return Out;
}


// -------------------------------------------------------------
// 4�н�: �ȼ����̴�, ���� ����
// -------------------------------------------------------------
float4 PS_pass4(VS_BUMP_OUTPUT In) : COLOR
{
    float3 n = normalize(tex2D(normalMap, In.Tex).rgb * 2.0f - 1.0f);
    float3 h = normalize(In.HalfVector);
    float3 l = normalize(In.LightDir);

    float nDotL = saturate(dot(n, l));
    float nDotH = saturate(dot(n, h));
    float power = (nDotL == 0.0f) ? 0.0f : pow(nDotH, g_shininess);

    float4 color = g_material.ambient * g_light.ambient
						+ (In.Diffuse * nDotL) 
						+ (In.Specular * power);

	return  color * tex2D(colorMap, In.Tex);
}



technique Scene
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS_pass3();
		PixelShader = compile ps_3_0 PS_pass3();
	}
}

	
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


	// �� + ��������
    pass P4
    {
        VertexShader = compile vs_3_0 VS_pass4();
		PixelShader  = compile ps_3_0 PS_pass4();
    }


}
