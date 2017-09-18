/*
 * Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */

#define NV_STEREO
#define SKY_DISTANCE 200000

cbuffer cbPerObject : register( b0 )
{
    row_major matrix    g_mWorldViewProjection : packoffset( c0 );
}

TextureCube g_EnvironmentTexture : register( t0 );
SamplerState g_sam : register( s0 );

struct SkyboxVS_Input
{
    float4 Pos : POSITION;
};

struct SkyboxVS_Output
{
    float4 Pos : SV_POSITION;
    float3 Tex : TEXCOORD0;
};

SkyboxVS_Output SkyboxVS( SkyboxVS_Input Input )
{
    SkyboxVS_Output Output;

#ifdef NV_STEREO
    Input.Pos.x *= 1.2f;
#endif
	Output.Pos = Input.Pos *SKY_DISTANCE;
    Output.Tex = normalize( mul(Input.Pos, g_mWorldViewProjection) );

    return Output;
}

float4 SkyboxPS( SkyboxVS_Output Input ) : SV_TARGET
{
    float4 color = g_EnvironmentTexture.Sample( g_sam, Input.Tex );
    return color;
}
