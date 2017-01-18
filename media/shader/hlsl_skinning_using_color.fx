
// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4 mWorld;
float4x4 mVP;		// ���ÿ��� �������������� ��ǥ��ȯ
float4x4 mWIT;
float3 vLightDir = {0, -1, 0};
float3 vEyePos;

// ���� ���.
float4 I_a = {0.3f, 0.3f, 0.3f, 0.0f}; // ambient
float4 I_d = {1.f, 1.f, 1.f, 0.0f}; // diffuse
float4 I_s = {1.f, 1.f, 1.f, 0.0f}; // diffuse

// �ݻ���
float4 K_a = {1.0f, 1.0f, 1.0f, 1.0f}; // ambient 
float4 K_d = {1.0f, 1.0f, 1.0f, 1.0f}; // diffuse

// �ȷ�Ʈ
float4x3 mPalette[ 64] : SKINBONEMATRIX3;


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


// -------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float4 Diffuse : COLOR0;
	float2 Tex : TEXCOORD0;
	float3 Eye : TEXCOORD1;
	float3 N : TEXCOORD2;
};


// -------------------------------------------------------------
// 1�н�:�������̴�
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
	float4 Pos : POSITION,          // ������
	float3 Normal : NORMAL,		// ��������
	float2 Tex : TEXCOORD0,		// �ؽ��� ��ǥ
	float4 Weights : BLENDWEIGHT,	// ���ؽ� ����ġ
	float4 BoneIndices : BLENDINDICES// �� �ε��� (4�� ����)
)
{
	VS_OUTPUT Out = (VS_OUTPUT)0; // ��µ�����
    
	// ��ǥ��ȯ
	float4x4 mWVP = mul(mWorld, mVP);

	// BoneIndices �� 0~1 ���� ������ �����ϸ��� �����̱� ������ 
	// rgba *  255 �� ����� ���ø����̼ǿ��� ������ BoneIndex�� 
	// ���� �� �ִ�. D3DCOLORtoUBYTE4() �Լ��� �� ������ �ϰ� �ȴ�.
	//
	// ARGB ���·� ���ø����̼� �ܿ��� ���� �� ���� 
	// D3DCOLORtoUBYTE4() �Լ��� ��ġ�鼭 BGRA ���·� �ٲ��.
	// ������ ������� Bone Index �� �����Ϸ��� wzyx ������ �����ؾ� �Ѵ�.
	int4 IndexVector = D3DCOLORtoUBYTE4(BoneIndices);

	float3 p = {0,0,0};
	float3 n = {0,0,0};

	p += mul(Pos, mPalette[ IndexVector.w]) * Weights.x;
	p += mul(Pos, mPalette[ IndexVector.z]) * Weights.y;
	p += mul(Pos, mPalette[ IndexVector.y]) * Weights.z;
	p += mul(Pos, mPalette[ IndexVector.x]) * Weights.w;

	n += mul(Normal, mPalette[ IndexVector.w]) * Weights.x;
	n += mul(Normal, mPalette[ IndexVector.z]) * Weights.y;
	n += mul(Normal, mPalette[ IndexVector.y]) * Weights.z;
	n += mul(Normal, mPalette[ IndexVector.x]) * Weights.w;

	Out.Pos = mul( float4(p,1), mWVP );
	n = normalize(n);

	// ���� ���� ���.
	float3 N = normalize( mul(n, (float3x3)mWIT) ); // ���� ��ǥ�迡���� ����.
	
	Out.N = N;
	Out.Eye = vEyePos - Pos.xyz;
	Out.Tex = Tex;
    
    return Out;
}


// -------------------------------------------------------------
// 1�н�:�ȼ����̴�
// -------------------------------------------------------------
float4 PS_pass0(VS_OUTPUT In) : COLOR
{
	float4 Out;

	float3 L = -vLightDir.xyz;
	float3 H = normalize(L + normalize(In.Eye));
	float3 N = normalize(In.N);

	Out = 	I_a * K_a
				+ I_d * K_d * max(0, dot(N,L));
				+ I_s * pow( max(0, dot(N,H)), 16);

	Out = Out * tex2D(colorMap, In.Tex);

	//Out = tex2D(colorMap, In.Tex);
    return Out;
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

}

