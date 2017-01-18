
// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4 mWorld;
float4x4 mVP;		// ���ÿ��� �������������� ��ǥ��ȯ
float4x4 mWIT;
float3 vEyePos;
float shininess = 90;
float4 globalAmbient = {0.2f, 0.2f, 0.2f, 1.0f};

// �ȷ�Ʈ
float4x3 mPalette[ 64];

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

// ------------------------------------------------------------
// ���ݻ��
// ------------------------------------------------------------
texture specularMapTexture;
sampler2D specularMap = sampler_state
{
    Texture = <specularMapTexture>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};


// ------------------------------------------------------------
// Self Illumination ��
// ------------------------------------------------------------
texture selfIllumMapTexture;
sampler2D selfIllumMap = sampler_state
{
    Texture = <selfIllumMapTexture>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};


struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
};


// �׸��� �� ��¿� ����ü.
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
// 1�н�:�������̴�
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
	float4 Pos : POSITION,          // ������
	float3 Normal : NORMAL,		// ��������
	float2 Tex : TEXCOORD0,		// �ؽ��� ��ǥ
	float4 Weights : TEXCOORD1,	// ���ؽ� ����ġ
	float4 BoneIndices : TEXCOORD2 // �� �ε��� (4�� ����)
)
{
	VS_OUTPUT Out = (VS_OUTPUT)0; // ��µ�����
    
	// ��ǥ��ȯ
	float4x4 mWVP = mul(mWorld, mVP);

	float3 p = {0,0,0};

	p += mul(Pos, mPalette[ BoneIndices.x]).xyz * Weights.x;
	p += mul(Pos, mPalette[ BoneIndices.y]).xyz * Weights.y;
	p += mul(Pos, mPalette[ BoneIndices.z]).xyz * Weights.z;
	p += mul(Pos, mPalette[ BoneIndices.w]).xyz * Weights.w;

	Out.Pos = mul( float4(p,1), mWVP );
	Out.Tex = Tex;
	    
    return Out;
}


// -------------------------------------------------------------
// 1�н�:�ȼ����̴�
// -------------------------------------------------------------
float4 PS_pass0(VS_OUTPUT In) : COLOR
{
	return tex2D(colorMap, In.Tex);;
}


// -------------------------------------------------------------
// 2�н�:�������̴�, �׸��� �� ���.
// -------------------------------------------------------------
VS_SHADOW_OUTPUT VS_pass1(
	float4 Pos : POSITION,          // ������
	float3 Normal : NORMAL,		// ��������
	float2 Tex : TEXCOORD0,		// �ؽ��� ��ǥ
	float4 Weights : TEXCOORD1,	// ���ؽ� ����ġ
	float4 BoneIndices : TEXCOORD2 // �� �ε��� (4�� ����)
)
{
	VS_SHADOW_OUTPUT Out = (VS_SHADOW_OUTPUT)0; // ��µ�����
    
	// ��ǥ��ȯ
	float4x4 mWVP = mul(mWorld, mVP);

	float3 p = {0,0,0};

	p += mul(Pos, mPalette[ BoneIndices.x]) * Weights.x;
	p += mul(Pos, mPalette[ BoneIndices.y]) * Weights.y;
	p += mul(Pos, mPalette[ BoneIndices.z]) * Weights.z;
	p += mul(Pos, mPalette[ BoneIndices.w]) * Weights.w;

	Out.Pos = mul( float4(p,1), mWVP );
	Out.Diffuse = float4(1,1,1,1);
    
    return Out;
}


// -------------------------------------------------------------
// 4�н�:�������̴�
// D3D9NormalMapping �ҽ��� ���� ��.
// http://www.dhpoware.com/demos/d3d9NormalMapping.html
// -------------------------------------------------------------
VS_BUMP_OUTPUT VS_pass4(
	float4 Pos : POSITION,          // ������
	float3 Normal : NORMAL,		// ��������
	float2 Tex : TEXCOORD0,		// �ؽ��� ��ǥ
	float4 Weights : TEXCOORD1,	// ���ؽ� ����ġ
	float4 BoneIndices : TEXCOORD2, // �� �ε��� (4�� ����)
	float3 tangent : TANGENT,		// tangent ����
	float3 binormal : BINORMAL 	// binormal ����
)
{
	VS_BUMP_OUTPUT Out = (VS_BUMP_OUTPUT)0; // ��µ�����
    
	float3 p = {0,0,0};
	float3 n = {0,0,0};

	p += mul(Pos, mPalette[ BoneIndices.x]).xyz * Weights.x;
	p += mul(Pos, mPalette[ BoneIndices.y]).xyz * Weights.y;
	p += mul(Pos, mPalette[ BoneIndices.z]).xyz * Weights.z;
	p += mul(Pos, mPalette[ BoneIndices.w]).xyz * Weights.w;

	n += mul(float4(Normal,0), mPalette[ BoneIndices.x]).xyz * Weights.x;
	n += mul(float4(Normal,0), mPalette[ BoneIndices.y]).xyz * Weights.y;
	n += mul(float4(Normal,0), mPalette[ BoneIndices.z]).xyz * Weights.z;
	n += mul(float4(Normal,0), mPalette[ BoneIndices.w]).xyz * Weights.w;

	//Out.Pos = mul( float4(p,1), mWVP );
	//n = normalize(n);

	float3 worldPos = mul(float4(p,1), mWorld).xyz;
	float3 lightDir = -light.dir;
	float3 viewDir = vEyePos - worldPos;
	float3 halfVector = normalize(normalize(lightDir) + normalize(viewDir));

			  n = normalize( mul(n, (float3x3)mWIT) ); // ���� ��ǥ�迡���� ����.
	float3 t = normalize( mul(tangent, (float3x3)mWIT) ); // ���� ��ǥ�迡���� ź��Ʈ
	float3 b = normalize( mul(binormal, (float3x3)mWIT) ); // ���� ��ǥ�迡���� ���̳��
	float3x3 tbnMatrix = float3x3(t.x, b.x, n.x,
	                              t.y, b.y, n.y,
	                              t.z, b.z, n.z);

	Out.Pos = mul( float4(worldPos,1), mVP );
	Out.Tex = Tex;
	Out.HalfVector = mul(halfVector, tbnMatrix);
	Out.LightDir = mul(lightDir, tbnMatrix);    
	Out.Diffuse = material.diffuse * light.diffuse;
	Out.Specular = material.specular * light.specular;

    return Out;
}


// -------------------------------------------------------------
// 4�н�:�ȼ����̴�
// -------------------------------------------------------------
float4 PS_pass4(VS_BUMP_OUTPUT In) : COLOR
{
	float3 n = normalize(tex2D(normalMap, In.Tex).rbg  * 2.0f - 1.0f);
    float3 h = normalize(In.HalfVector);
    float3 l = normalize(In.LightDir);

    float nDotL = saturate(dot(n, l));
    float nDotH = saturate(dot(n, h));
    float power = (nDotL == 0.0f) ? 0.0f : pow(nDotH, shininess);

    float4 color = material.ambient * (globalAmbient + light.ambient) 
						+ (In.Diffuse * nDotL) 
						+ (In.Specular * power);

	return  color * tex2D(colorMap, In.Tex);
}


// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // ���̴�
        VertexShader = compile vs_3_0 VS_pass0();
		PixelShader  = compile ps_3_0 PS_pass0();
    }

    pass P1
    {
        // �׸��� �� ���̴�
        VertexShader = compile vs_3_0 VS_pass1();
    }

    pass P2
    {
	// �ƹ��͵� ����.
    }

    pass P3
    {
	// �ƹ��͵� ����.
    }

    pass P4
    {
		// ��Ű�� �ִϸ��̼� + ���� ��.
        VertexShader = compile vs_3_0 VS_pass0();
		PixelShader  = compile ps_3_0 PS_pass0();
    }

}

