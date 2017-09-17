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

//------------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------------

#define PATCH_BLEND_BEGIN  800
#define PATCH_BLEND_END   20000

// Shading params. Need to pad the structure for 16 bytes alignment
cbuffer cbShading : register(b2)
{
    float3  g_SkyColor;
    float  g_TexelLength_x2;
    float3  g_WaterbodyColor;
    float  g_UVScale;
    float3  g_PerlinAmplitude;
    float  g_UVOffset;
    float3  g_PerlinOctave;
    float  g_PerlinSize;
    float3  g_PerlinGradient;
    float  g_Shineness;
    float3  g_BendParam;
    float  unused0;
    float3  g_SunDir;
    float  unused1;
    float3  g_SunColor;
    float  unused2;
};

// Per frame constants
cbuffer cbChangePerFrame : register(b3)
{
    float4  g_LogoPlacement;
}

// Per draw call constants
cbuffer cbChangePerCall : register(b4)
{
    // Transform matrix
    float4x4 g_matLocal;
    float4x4 g_matWorldViewProj;
    float2  g_UVBase;
    float2  g_PerlinMovement;
    float3  g_LocalEye;
}



//-----------------------------------------------------------------------------------
// Texture & Samplers
//-----------------------------------------------------------------------------------
Texture2D g_texDisplacement : register(t0);
Texture2D g_texPerlin   : register(t1);
Texture2D g_texGradient  : register(t2);
Texture1D g_texFresnel  : register(t3);
TextureCube g_texReflectCube : register(t4);
Texture2D g_texLogo   : register(t5);

// Displacement map for near-sight height and choppy field
SamplerState g_samplerDisplacement : register(s0);

// Perlin noise for composing distant waves
SamplerState g_samplerPerlin : register(s1);

// Normal map for lighting
SamplerState g_samplerGradient : register(s2);

// Blending map for ocean color
SamplerState g_samplerFresnel : register(s3);

// Sky cubemap
SamplerState g_samplerCube  : register(s4);

struct VS_OUTPUT
{
    float4 Position  : SV_POSITION;
    float2 TexCoord  : TEXCOORD0;
    float3 LocalPos  : TEXCOORD1;
};

//-----------------------------------------------------------------------------
// Name: OceanSurfVS
// Type: Vertex shader                                      
// Desc: 
//-----------------------------------------------------------------------------
VS_OUTPUT OceanSurfVS(float2 vPos : POSITION)
{
    VS_OUTPUT Output;

    // Local position
    float4 pos_local = mul(float4(vPos, 0, 1), g_matLocal);
    // UV
    float2 uv_local = pos_local.xy * g_UVScale + g_UVOffset;

    // Blend displacement to avoid tiling artifact
    float3 eye_vec = pos_local - g_LocalEye;
    float dist_2d = length(eye_vec.xy);
    float blend_factor = (PATCH_BLEND_END - dist_2d) / (PATCH_BLEND_END - PATCH_BLEND_BEGIN);
    blend_factor = clamp(blend_factor, 0, 1);

    // Add perlin noise to distant patch
    float2 perlin_tc = uv_local * g_PerlinSize + g_UVBase;
    float perlin_0 = g_texPerlin.SampleLevel(g_samplerPerlin, perlin_tc * g_PerlinOctave.x + g_PerlinMovement, 0).w;
    float perlin_1 = g_texPerlin.SampleLevel(g_samplerPerlin, perlin_tc * g_PerlinOctave.y + g_PerlinMovement, 0).w;
    float perlin_2 = g_texPerlin.SampleLevel(g_samplerPerlin, perlin_tc * g_PerlinOctave.z + g_PerlinMovement, 0).w;
    float perlin = perlin_0 * g_PerlinAmplitude.x + perlin_1 * g_PerlinAmplitude.y + perlin_2 * g_PerlinAmplitude.z;

    // Displacement map
    float3 displacement = g_texDisplacement.SampleLevel(g_samplerDisplacement, uv_local, 0).xyz;
    displacement = lerp(float3(0, 0, perlin), displacement, blend_factor);
    pos_local.xyz += displacement;

    // Transform
    Output.Position = mul(pos_local, g_matWorldViewProj);
    Output.LocalPos = pos_local;

    // Pass thru texture coordinate
    Output.TexCoord = uv_local;

    return Output; 
}

struct VS_LOGO_OUTPUT
{
    float4 Position : SV_Position;   
    float2 TexCoord : TEXCOORD0;
};

VS_LOGO_OUTPUT DisplayLogoVS(uint vert_id : SV_VertexID)
{
    VS_LOGO_OUTPUT Output;

    // Generate a quad from vertex ids.
    float2 pos = float2(vert_id & 1, (vert_id & 2) >> 1);
    Output.Position = float4(pos * g_LogoPlacement.xy + g_LogoPlacement.zw, 0, 1);
    Output.TexCoord = float2(pos.x, 1 - pos.y);

    return Output;
}


//-----------------------------------------------------------------------------
// Name: OceanSurfPS
// Type: Pixel shader                                      
// Desc: 
//-----------------------------------------------------------------------------
float4 OceanSurfPS(VS_OUTPUT In) : SV_Target
{
    // Calculate eye vector.
    float3 eye_vec = g_LocalEye - In.LocalPos;
    float3 eye_dir = normalize(eye_vec);


    // --------------- Blend perlin noise for reducing the tiling artifacts

    // Blend displacement to avoid tiling artifact
    float dist_2d = length(eye_vec.xy);
    float blend_factor = (PATCH_BLEND_END - dist_2d) / (PATCH_BLEND_END - PATCH_BLEND_BEGIN);
    //blend_factor = clamp(blend_factor * blend_factor * blend_factor, 0, 1);
	blend_factor = clamp(blend_factor * blend_factor * blend_factor, 0.3, 1);

    // Compose perlin waves from three octaves
    float2 perlin_tc = In.TexCoord * g_PerlinSize + g_UVBase;
    float2 perlin_0 = g_texPerlin.Sample(g_samplerPerlin, perlin_tc * g_PerlinOctave.x + g_PerlinMovement).xy;
    float2 perlin_1 = g_texPerlin.Sample(g_samplerPerlin, perlin_tc * g_PerlinOctave.y + g_PerlinMovement).xy;
    float2 perlin_2 = g_texPerlin.Sample(g_samplerPerlin, perlin_tc * g_PerlinOctave.z + g_PerlinMovement).xy;
    float2 perlin = (perlin_0 * g_PerlinGradient.x + perlin_1 * g_PerlinGradient.y + perlin_2 * g_PerlinGradient.z);


    // --------------- Water body color

    float2 grad = g_texGradient.Sample(g_samplerGradient, In.TexCoord).xy;
    grad = lerp(perlin, grad, blend_factor);

    // Calculate normal here.
    float3 normal = normalize(float3(grad, g_TexelLength_x2));
    // Reflected ray
    float3 reflect_vec = reflect(-eye_dir, normal);
    // dot(N, V)
    float cos_angle = dot(normal, eye_dir);

    // A coarse way to handle transmitted light
    float3 body_color = g_WaterbodyColor;


    // --------------- Reflected color

    // ramp.x for fresnel term. ramp.y for sky blending
    float4 ramp = g_texFresnel.Sample(g_samplerFresnel, cos_angle).xyzw;
    // A worksaround to deal with "indirect reflection vectors" (which are rays requiring multiple
    // reflections to reach the sky).
    if (reflect_vec.z < g_BendParam.x)
        ramp = lerp(ramp, g_BendParam.z, (g_BendParam.x - reflect_vec.z)/(g_BendParam.x - g_BendParam.y));
    reflect_vec.z = max(0, reflect_vec.z);

    float3 reflection = g_texReflectCube.Sample(g_samplerCube, reflect_vec).xyz;
    // Hack bit: making higher contrast
    reflection = reflection * reflection * 2.5f;

    // Blend with predefined sky color
    float3 reflected_color = lerp(g_SkyColor, reflection, ramp.y);

    // Combine waterbody color and reflected color
    float3 water_color = lerp(body_color, reflected_color, ramp.x);


    // --------------- Sun spots

    float cos_spec = clamp(dot(reflect_vec, g_SunDir), 0, 1);
    float sun_spot = pow(cos_spec, g_Shineness);
    water_color += g_SunColor * sun_spot;

	// fog
	//water_color = lerp(float3(115/255.f, 115/255.f, 115/255.f), water_color, blend_factor);

    return float4(water_color, 1);
	//return float4(0,0,0, 1);
}

//-----------------------------------------------------------------------------
// Name: WireframePS
// Type: Pixel shader                                      
// Desc: 
//-----------------------------------------------------------------------------
float4 WireframePS() : SV_Target
{
    return float4(0.8f, 0.8f, 0.9f, 1);
}

float4 DisplayLogoPS(VS_LOGO_OUTPUT In) : SV_Target
{
    return g_texLogo.Sample(g_samplerFresnel, In.TexCoord);
}
