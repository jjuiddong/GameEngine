// Copyright (c) 2011 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, NONINFRINGEMENT,IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA 
// OR ITS SUPPLIERS BE  LIABLE  FOR  ANY  DIRECT, SPECIAL,  INCIDENTAL,  INDIRECT,  OR  
// CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS 
// OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY 
// OTHER PECUNIARY LOSS) ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, 
// EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
// Please direct any bugs or questions to SDKFeedback@nvidia.com

#include "common.fx"

//--------------------------------------------------------------------------------------
// Textures
//--------------------------------------------------------------------------------------

// static textures
Texture2D g_HeightfieldTexture;
Texture2D g_LayerdefTexture;
Texture2D g_RockBumpTexture;
Texture2D g_RockMicroBumpTexture;
Texture2D g_RockDiffuseTexture;
Texture2D g_SandBumpTexture;
Texture2D g_SandMicroBumpTexture;
Texture2D g_SandDiffuseTexture;
Texture2D g_GrassDiffuseTexture;
Texture2D g_SlopeDiffuseTexture;
Texture2D g_WaterBumpTexture;
Texture2D g_DepthMapTexture;

// rendertarget textures
Texture2D g_ReflectionTexture;
Texture2D g_RefractionTexture;
Texture2D g_RefractionDepthTextureResolved;
Texture2D g_WaterNormalMapTexture;
Texture2D g_DepthTexture;
Texture2D g_MainTexture;
Texture2DMS<float,1> g_RefractionDepthTextureMS1;
Texture2DMS<float,2> g_RefractionDepthTextureMS2;
Texture2DMS<float,4> g_RefractionDepthTextureMS4;



//--------------------------------------------------------------------------------------
// Shader Inputs/Outputs
//--------------------------------------------------------------------------------------
struct VSIn_Diffuse
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal   : NORMAL;
};

struct PSIn_Diffuse
{
    float4 position     : SV_Position;
    centroid float2 texcoord     : TEXCOORD0;
    centroid float3 normal       : NORMAL;
    centroid float3 positionWS   : TEXCOORD1;
	centroid float4 layerdef		: TEXCOORD2;
	centroid float4 depthmap_scaler: TEXCOORD3;
};

struct PSIn_Quad
{
    float4 position     : SV_Position;
    float2 texcoord     : TEXCOORD0;
};

struct VSIn_Default
{
	float4 position : POSITION;
    float2 texcoord  : TEXCOORD;
};


struct DUMMY
{
	float Dummmy : DUMMY;
};

struct HSIn_Heightfield
{
    float2 origin   : ORIGIN;
    float2 size     : SIZE;
};


struct PatchData
{
    float Edges[4]  : SV_TessFactor;
    float Inside[2]	: SV_InsideTessFactor;

	float2 origin   : ORIGIN;
    float2 size     : SIZE;
};

//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------

shared cbuffer cb0
{

	// rendering control variables
	float		g_RenderCaustics;
	float		g_UseDynamicLOD;
	float		g_FrustumCullInHS;
	float       g_DynamicTessFactor;
	float       g_StaticTessFactor;
	float		g_TerrainBeingRendered;
	float		g_HalfSpaceCullSign;
	float		g_HalfSpaceCullPosition;
	float		g_SkipCausticsCalculation;
	int			g_MSSamples;

	// view/time dependent variables
	float4x4    g_ModelViewMatrix;
    float4x4    g_ModelViewProjectionMatrix;
	float4x4	g_ModelViewProjectionMatrixInv;
    float4x4    g_LightModelViewProjectionMatrix;
    float4x4    g_LightModelViewProjectionMatrixInv;
    float3      g_CameraPosition;
    float3      g_CameraDirection;


	float3      g_LightPosition;
	float2      g_WaterBumpTexcoordShift;
	float2      g_ScreenSizeInv;
	float	    g_MainBufferSizeMultiplier;
	float		g_ZNear;
	float		g_ZFar;

	// constants defining visual appearance
	float2		g_DiffuseTexcoordScale={130.0,130.0};
	float2		g_RockBumpTexcoordScale={10.0,10.0};
	float		g_RockBumpHeightScale=3.0;
	float2		g_SandBumpTexcoordScale={3.5,3.5};
	float		g_SandBumpHeightScale=0.5;
	float       g_TerrainSpecularIntensity=0.5;
	float2		g_WaterMicroBumpTexcoordScale={225,225};
	float2		g_WaterBumpTexcoordScale={7,7};
	float		g_WaterHeightBumpScale=1.0f;
	float3      g_WaterDeepColor={0.1,0.4,0.7};
	float3      g_WaterScatterColor={0.3,0.7,0.6};
	float3      g_WaterSpecularColor={1,1,1};
	float       g_WaterSpecularIntensity=350.0;

	float       g_WaterSpecularPower=1000;
	float2      g_WaterColorIntensity={0.1,0.2};
	float3      g_AtmosphereBrightColor={1.0,1.1,1.4};
	float3      g_AtmosphereDarkColor={0.6,0.6,0.7};
	float		g_FogDensity = 1.0f/700.0f;
    float2		g_HeightFieldOrigin = float2(0, 0);
	float		g_HeightFieldSize = 512;
};

//--------------------------------------------------------------------------------------
// Misc functions
//--------------------------------------------------------------------------------------

// calculating tessellation factor. It is either constant or hyperbolic depending on g_UseDynamicLOD switch
float CalculateTessellationFactor(float distance)
{
	return lerp(g_StaticTessFactor,g_DynamicTessFactor*(1/(0.015*distance)),g_UseDynamicLOD);
}

// to avoid vertex swimming while tessellation varies, one can use mipmapping for displacement maps
// it's not always the best choice, but it effificiently suppresses high frequencies at zero cost
float CalculateMIPLevelForDisplacementTextures(float distance)
{
	return log2(128/CalculateTessellationFactor(distance));
}

// primitive simulation of non-uniform atmospheric fog
float3 CalculateFogColor(float3 pixel_to_light_vector, float3 pixel_to_eye_vector)
{
	return lerp(g_AtmosphereDarkColor,g_AtmosphereBrightColor,0.5*dot(pixel_to_light_vector,-pixel_to_eye_vector)+0.5);
}

// constructing the displacement amount and normal for water surface geometry
float4 CombineWaterNormal(float3 world_position)
{
	float4 water_normal=float4(0.0,4.0,0.0,0.0);
	float water_miplevel;
	float distance_to_camera;
	float4 texvalue;
	float texcoord_scale=1.0;
	float height_disturbance_scale=1.0;
	float normal_disturbance_scale=1.0;
	float2 tc;
	float2 variance={1.0,1.0};

	// calculating MIP level for water texture fetches
	distance_to_camera=length(g_CameraPosition-world_position);
	water_miplevel= CalculateMIPLevelForDisplacementTextures(distance_to_camera)/2.0-2.0;
	tc=(world_position.xz*g_WaterBumpTexcoordScale/g_HeightFieldSize);

	// fetching water heightmap
	for(float i=0;i<5;i++)
	{
		texvalue=g_WaterBumpTexture.SampleLevel(SamplerLinearWrap, tc*texcoord_scale+g_WaterBumpTexcoordShift*0.03*variance,water_miplevel).rbga;
		variance.x*=-1.0;
		water_normal.xz+=(2*texvalue.xz-float2(1.0,1.0))*normal_disturbance_scale;
		water_normal.w += (texvalue.w-0.5)*height_disturbance_scale;
		texcoord_scale*=1.4;
		height_disturbance_scale*=0.65;
		normal_disturbance_scale*=0.65;
	}
	water_normal.w*=g_WaterHeightBumpScale;
	return float4(normalize(water_normal.xyz),water_normal.w);
}

// constructing water surface normal for water refraction caustics
float3 CombineSimplifiedWaterNormal(float3 world_position, float mip_level)
{
	float3 water_normal=float3(0.0,4.0,0.0);

	float water_miplevel;
	float distance_to_camera;
	float4 texvalue;
	float texcoord_scale=1.0;
	float normal_disturbance_scale=1.0;
	float2 tc;
	float2 variance={1.0,1.0};

	tc=(world_position.xz*g_WaterBumpTexcoordScale/g_HeightFieldSize);
	
	// need more high frequensy details for caustics, so summing more "octaves"
	for(float i=0;i<8;i++)
	{
		texvalue=g_WaterBumpTexture.SampleLevel(SamplerLinearWrap, tc*texcoord_scale+g_WaterBumpTexcoordShift*0.03*variance,mip_level/*+i*/).rbga;
		variance.x*=-1.0;
		water_normal.xz+=(2*texvalue.xz-float2(1,1))*normal_disturbance_scale;
		texcoord_scale*=1.4;
		normal_disturbance_scale*=0.85;
	}
	return normalize(water_normal);
}

// calculating water refraction caustics intensity
float CalculateWaterCausticIntensity(float3 worldpos)
{

	float distance_to_camera=length(g_CameraPosition-worldpos);

	float2 refraction_disturbance;
	float3 n;
	float m=0.2;
	float cc=0;
	float k=0.15;
	float water_depth=0.5-worldpos.y;

	float3 pixel_to_light_vector=normalize(g_LightPosition-worldpos);

	worldpos.xz-=worldpos.y*pixel_to_light_vector.xz;
	float3 pixel_to_water_surface_vector=pixel_to_light_vector*water_depth;
	float3 refracted_pixel_to_light_vector;

	// tracing approximately refracted rays back to light
	for(float i=-3; i<=3;i+=1)
		for(float j=-3; j<=3;j+=1)
		{
			n=2.0f*g_WaterNormalMapTexture.SampleLevel(SamplerLinearWrap,(worldpos.xz-g_CameraPosition.xz-float2(200.0,200.0)+float2(i*k,j*k)*m*water_depth)/400.0,0).rgb-float3(1.0f,1.0f,1.0f);
			refracted_pixel_to_light_vector=m*(pixel_to_water_surface_vector+float3(i*k,0,j*k))-0.5*float3(n.x,0,n.z);
			cc+=0.05*max(0,pow(max(0,dot(normalize(refracted_pixel_to_light_vector),normalize(pixel_to_light_vector))),500.0f));
		}
	return cc;
}


float GetRefractionDepth(float2 position)
{
	return g_RefractionDepthTextureResolved.SampleLevel(SamplerLinearClamp,position,0).r;
}

float GetConservativeRefractionDepth(float2 position)
{
	float result =      g_RefractionDepthTextureResolved.SampleLevel(SamplerPointClamp,position + 2.0*float2(g_ScreenSizeInv.x,g_ScreenSizeInv.y),0).r;
	result = min(result,g_RefractionDepthTextureResolved.SampleLevel(SamplerPointClamp,position + 2.0*float2(g_ScreenSizeInv.x,-g_ScreenSizeInv.y),0).r);
	result = min(result,g_RefractionDepthTextureResolved.SampleLevel(SamplerPointClamp,position + 2.0*float2(-g_ScreenSizeInv.x,g_ScreenSizeInv.y),0).r);
	result = min(result,g_RefractionDepthTextureResolved.SampleLevel(SamplerPointClamp,position + 2.0*float2(-g_ScreenSizeInv.x,-g_ScreenSizeInv.y),0).r);
	return result;
}


//--------------------------------------------------------------------------------------
// Heightfield shaders
//--------------------------------------------------------------------------------------

HSIn_Heightfield PassThroughVS(float4 PatchParams : PATCH_PARAMETERS)
{
    HSIn_Heightfield output;
    output.origin = PatchParams.xy;
    output.size = PatchParams.zw;
    return output;
}

PatchData PatchConstantHS( InputPatch<HSIn_Heightfield, 1> inputPatch )
{    
    PatchData output;

	float distance_to_camera;
	float tesselation_factor;
	float inside_tessellation_factor=0;
	float in_frustum=0;

	output.origin = inputPatch[0].origin;
	output.size = inputPatch[0].size;

	float2 texcoord0to1 = (inputPatch[0].origin + inputPatch[0].size/2.0)/g_HeightFieldSize;
	texcoord0to1.y=1-texcoord0to1.y;
	
	// conservative frustum culling
	float3 patch_center=float3(inputPatch[0].origin.x+inputPatch[0].size.x*0.5,g_TerrainBeingRendered*g_HeightfieldTexture.SampleLevel(SamplerLinearWrap, texcoord0to1,0).w,inputPatch[0].origin.y+inputPatch[0].size.y*0.5);
	float3 camera_to_patch_vector =  patch_center-g_CameraPosition;
	float3 patch_to_camera_direction_vector = g_CameraDirection*dot(camera_to_patch_vector,g_CameraDirection)-camera_to_patch_vector;
	float3 patch_center_realigned=patch_center+normalize(patch_to_camera_direction_vector)*min(2*inputPatch[0].size.x,length(patch_to_camera_direction_vector));
	float4 patch_screenspace_center = mul(float4(patch_center_realigned, 1.0), g_ModelViewProjectionMatrix);

	if(((patch_screenspace_center.x/patch_screenspace_center.w>-1.0) && (patch_screenspace_center.x/patch_screenspace_center.w<1.0) 
		&& (patch_screenspace_center.y/patch_screenspace_center.w>-1.0) && (patch_screenspace_center.y/patch_screenspace_center.w<1.0)
		&& (patch_screenspace_center.w>0)) || (length(patch_center-g_CameraPosition)<2*inputPatch[0].size.x))
	{
		in_frustum=1;
	}

	if((in_frustum) || (g_FrustumCullInHS ==0))
	{
		distance_to_camera=length(g_CameraPosition.xz-inputPatch[0].origin-float2(0,inputPatch[0].size.y*0.5));
		tesselation_factor=CalculateTessellationFactor(distance_to_camera);
		output.Edges[0] =  tesselation_factor;
		inside_tessellation_factor+=tesselation_factor;


		distance_to_camera=length(g_CameraPosition.xz-inputPatch[0].origin-float2(inputPatch[0].size.x*0.5,0));
		tesselation_factor=CalculateTessellationFactor(distance_to_camera);
		output.Edges[1] =  tesselation_factor;
		inside_tessellation_factor+=tesselation_factor;

		distance_to_camera=length(g_CameraPosition.xz-inputPatch[0].origin-float2(inputPatch[0].size.x,inputPatch[0].size.y*0.5));
		tesselation_factor=CalculateTessellationFactor(distance_to_camera);
		output.Edges[2] =  tesselation_factor;
		inside_tessellation_factor+=tesselation_factor;

		distance_to_camera=length(g_CameraPosition.xz-inputPatch[0].origin-float2(inputPatch[0].size.x*0.5,inputPatch[0].size.y));
		tesselation_factor=CalculateTessellationFactor(distance_to_camera);
		output.Edges[3] =  tesselation_factor;
		inside_tessellation_factor+=tesselation_factor;
		output.Inside[0] = output.Inside[1] = inside_tessellation_factor*0.25;
	}
	else
	{
		output.Edges[0]=-1;
		output.Edges[1]=-1;
		output.Edges[2]=-1;
		output.Edges[3]=-1;
		output.Inside[0]=-1;
		output.Inside[1]=-1;
	}

    return output;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(1)]
[patchconstantfunc("PatchConstantHS")]
DUMMY PatchHS( InputPatch<HSIn_Heightfield, 1> inputPatch )
{
    return (DUMMY)0;
}


//--------------------------------------------------------------------------------------
// Water shaders
//--------------------------------------------------------------------------------------

[domain("quad")]
PSIn_Diffuse WaterPatchDS(    PatchData input, 
                                    float2 uv : SV_DomainLocation,
                                    OutputPatch<DUMMY, 1> inputPatch )
{
    PSIn_Diffuse output;
	float3 vertexPosition;
	float2 texcoord0to1 = (input.origin + uv * input.size)/g_HeightFieldSize;
	float4 water_normal;
	float4 depthmap_scaler;

	// getting rough estimate of water depth from depth map texture 
	//depthmap_scaler = g_DepthMapTexture.SampleLevel(SamplerLinearWrap, float2(texcoord0to1.x,1-texcoord0to1.y),0);
	//depthmap_scaler = float4(1, 1, 1, 1);
	depthmap_scaler = float4(1, 1, 1, 1);
	
	// calculating water surface geometry position and normal
	vertexPosition.xz = input.origin + uv * input.size;
	vertexPosition.y  = -g_WaterHeightBumpScale/2;
	water_normal=CombineWaterNormal(vertexPosition.xyz);

	// fading out displacement and normal disturbance near shores by 60%
	water_normal.xyz=lerp(float3(0,1,0),normalize(water_normal.xyz),0.4+0.6*depthmap_scaler.g);
	vertexPosition.y+=water_normal.w*g_WaterHeightBumpScale*(0.4+0.6*depthmap_scaler.g);
	vertexPosition.xz-=(water_normal.xz)*0.5*(0.4+0.6*depthmap_scaler.g);

	// writing output params
    output.position = mul(float4(vertexPosition, 1.0), g_ModelViewProjectionMatrix);
    output.texcoord = texcoord0to1*g_WaterMicroBumpTexcoordScale+g_WaterBumpTexcoordShift*0.07;
	output.normal = normalize(water_normal.xyz);
	output.depthmap_scaler = depthmap_scaler;
	output.positionWS = vertexPosition;
    return output;
}

float4 WaterPatchPS(PSIn_Diffuse input) : SV_Target
{
	float4 color;
	float3 pixel_to_light_vector = normalize(g_LightPosition-input.positionWS);
	float3 pixel_to_eye_vector = normalize(g_CameraPosition-input.positionWS);
	float3 reflected_eye_to_pixel_vector;
	float3 microbump_normal; 
	float3x3 normal_rotation_matrix;

	float fresnel_factor;
	float diffuse_factor;
	float specular_factor;
	float scatter_factor;
	float4 refraction_color;
	float4 reflection_color;
	float4 disturbance_eyespace;

	float water_depth;
	float4 water_color;

	// calculating pixel position in light space
	float4 positionLS = mul(float4(input.positionWS,1),g_LightModelViewProjectionMatrix);
	positionLS.xyz/=positionLS.w;
	positionLS.x=(positionLS.x+1)*0.5;
	positionLS.y=(1-positionLS.y)*0.5;

	// calculating shadow multiplier to be applied to diffuse/scatter/specular light components
	float dsf=1.0f/4096.0f;
	float shadow_factor=0.2*g_DepthTexture.SampleCmp(SamplerDepthAnisotropic,positionLS.xy,positionLS.z* 0.995f).r;
	shadow_factor+=0.2*g_DepthTexture.SampleCmp(SamplerDepthAnisotropic,positionLS.xy+float2(dsf,dsf),positionLS.z* 0.995f).r;
	shadow_factor+=0.2*g_DepthTexture.SampleCmp(SamplerDepthAnisotropic,positionLS.xy+float2(-dsf,dsf),positionLS.z* 0.995f).r;
	shadow_factor+=0.2*g_DepthTexture.SampleCmp(SamplerDepthAnisotropic,positionLS.xy+float2(dsf,-dsf),positionLS.z* 0.995f).r;
	shadow_factor+=0.2*g_DepthTexture.SampleCmp(SamplerDepthAnisotropic,positionLS.xy+float2(-dsf,-dsf),positionLS.z* 0.995f).r;

	// need more high frequency bumps for plausible water surface, so creating normal defined by 2 instances of same bump texture
	microbump_normal = normalize(2*g_WaterBumpTexture.Sample(SamplerAnisotropicWrap,input.texcoord-g_WaterBumpTexcoordShift*0.2).gbr - float3 (1,-8,1));
	microbump_normal+= normalize(2*g_WaterBumpTexture.Sample(SamplerAnisotropicWrap,input.texcoord*0.5+g_WaterBumpTexcoordShift*0.05).gbr - float3 (1,-8,1));

	// calculating base normal rotation matrix
	normal_rotation_matrix[1]=input.normal.xyz;
	normal_rotation_matrix[2]=normalize(cross(float3(0.0,0.0,-1.0),normal_rotation_matrix[1]));
	normal_rotation_matrix[0]=normalize(cross(normal_rotation_matrix[2],normal_rotation_matrix[1]));

	// applying base normal rotation matrix to high frequency bump normal
	microbump_normal=mul(normalize(microbump_normal),normal_rotation_matrix);

	
	// simulating scattering/double refraction: light hits the side of wave, travels some distance in water, and leaves wave on the other side
	// it's difficult to do it physically correct without photon mapping/ray tracing, so using simple but plausible emulation below
	
	// only the crests of water waves generate double refracted light
	scatter_factor=2.5*max(0,input.positionWS.y*0.25+0.25);

	// the waves that lie between camera and light projection on water plane generate maximal amount of double refracted light 
	scatter_factor*=shadow_factor*pow(max(0.0,dot(normalize(float3(pixel_to_light_vector.x,0.0,pixel_to_light_vector.z)),-pixel_to_eye_vector)),2.0);
	
	// the slopes of waves that are oriented back to light generate maximal amount of double refracted light 
	scatter_factor*=pow(max(0.0,1.0-dot(pixel_to_light_vector,microbump_normal)),8.0);
	
	// water crests gather more light than lobes, so more light is scattered under the crests
	scatter_factor+=shadow_factor*1.5*g_WaterColorIntensity.y*max(0,input.positionWS.y+1)*
		// the scattered light is best seen if observing direction is normal to slope surface
		max(0,dot(pixel_to_eye_vector,microbump_normal))*
		// fading scattered light out at distance and if viewing direction is vertical to avoid unnatural look
		max(0,1-pixel_to_eye_vector.y)*(300.0/(300+length(g_CameraPosition-input.positionWS)));

	// fading scatter out by 90% near shores so it looks better
	scatter_factor*=0.1+0.9*input.depthmap_scaler.g;

	// calculating fresnel factor 
	float r=(1.2-1.0)/(1.2+1.0);
	fresnel_factor = max(0.0,min(1.0,r+(1.0-r)*pow(1.0-dot(microbump_normal,pixel_to_eye_vector),4)));

	// calculating specular factor
	reflected_eye_to_pixel_vector=-pixel_to_eye_vector+2*dot(pixel_to_eye_vector,microbump_normal)*microbump_normal;
	specular_factor=shadow_factor*fresnel_factor*pow(max(0,dot(pixel_to_light_vector,reflected_eye_to_pixel_vector)),g_WaterSpecularPower);

	// calculating diffuse intensity of water surface itself
	diffuse_factor=g_WaterColorIntensity.x+g_WaterColorIntensity.y*max(0,dot(pixel_to_light_vector,microbump_normal));

	// calculating disturbance which has to be applied to planar reflections/refractions to give plausible results
	disturbance_eyespace=mul(float4(microbump_normal.x,0,microbump_normal.z,0),g_ModelViewMatrix);

	float2 reflection_disturbance=float2(disturbance_eyespace.x,disturbance_eyespace.z)*0.03;
	float2 refraction_disturbance=float2(-disturbance_eyespace.x,disturbance_eyespace.y)*0.05*
		// fading out reflection disturbance at distance so reflection doesn't look noisy at distance
		(20.0/(20+length(g_CameraPosition-input.positionWS)));
	
	// calculating correction that shifts reflection up/down according to water wave Y position
	float4 projected_waveheight = mul(float4(input.positionWS.x,input.positionWS.y,input.positionWS.z,1),g_ModelViewProjectionMatrix);
	float waveheight_correction=-0.5*projected_waveheight.y/projected_waveheight.w;
	projected_waveheight = mul(float4(input.positionWS.x,-0.8,input.positionWS.z,1),g_ModelViewProjectionMatrix);
	waveheight_correction+=0.5*projected_waveheight.y/projected_waveheight.w;
	reflection_disturbance.y=max(-0.15,waveheight_correction+reflection_disturbance.y);

	// picking refraction depth at non-displaced point, need it to scale the refraction texture displacement amount according to water depth
	float refraction_depth=GetRefractionDepth(input.position.xy*g_ScreenSizeInv);
	refraction_depth=g_ZFar*g_ZNear/(g_ZFar-refraction_depth*(g_ZFar-g_ZNear));
	float4 vertex_in_viewspace=mul(float4(input.positionWS,1),g_ModelViewMatrix);
	water_depth=refraction_depth-vertex_in_viewspace.z;
	float nondisplaced_water_depth=water_depth;
	
	// scaling refraction texture displacement amount according to water depth, with some limit
	refraction_disturbance*=min(2,water_depth);

	// picking refraction depth again, now at displaced point, need it to calculate correct water depth
	refraction_depth=GetRefractionDepth(input.position.xy*g_ScreenSizeInv+refraction_disturbance);
	refraction_depth=g_ZFar*g_ZNear/(g_ZFar-refraction_depth*(g_ZFar-g_ZNear));
	vertex_in_viewspace=mul(float4(input.positionWS,1),g_ModelViewMatrix);
	water_depth=refraction_depth-vertex_in_viewspace.z;

	// zeroing displacement for points where displaced position points at geometry which is actually closer to the camera than the water surface
	float conservative_refraction_depth=GetConservativeRefractionDepth(input.position.xy*g_ScreenSizeInv+refraction_disturbance);
	conservative_refraction_depth=g_ZFar*g_ZNear/(g_ZFar-conservative_refraction_depth*(g_ZFar-g_ZNear));
	vertex_in_viewspace=mul(float4(input.positionWS,1),g_ModelViewMatrix);
	float conservative_water_depth=conservative_refraction_depth-vertex_in_viewspace.z;

	if(conservative_water_depth<0)
	{
		refraction_disturbance=0;
		water_depth=nondisplaced_water_depth;
	}
	water_depth=max(0,water_depth);

	// getting reflection and refraction color at disturbed texture coordinates
	reflection_color=g_ReflectionTexture.SampleLevel(SamplerLinearClamp,float2(input.position.x*g_ScreenSizeInv.x,1.0-input.position.y*g_ScreenSizeInv.y)+reflection_disturbance,0);
	refraction_color=g_RefractionTexture.SampleLevel(SamplerLinearClamp,input.position.xy*g_ScreenSizeInv+refraction_disturbance,0);

	// calculating water surface color and applying atmospheric fog to it
	water_color=diffuse_factor*float4(g_WaterDeepColor,1);
	water_color.rgb=lerp(CalculateFogColor(pixel_to_light_vector,pixel_to_eye_vector).rgb,water_color.rgb,min(1,exp(-length(g_CameraPosition-input.positionWS)*g_FogDensity)));
	
	// fading fresnel factor to 0 to soften water surface edges
	fresnel_factor*=min(1,water_depth*5.0);

	// fading refraction color to water color according to distance that refracted ray travels in water 
	refraction_color=lerp(water_color,refraction_color,min(1,1.0*exp(-water_depth/8.0)));
	
	// combining final water color
	color.rgb=lerp(refraction_color.rgb,reflection_color.rgb,fresnel_factor);
	color.rgb+=g_WaterSpecularIntensity*specular_factor*g_WaterSpecularColor*fresnel_factor;
	color.rgb+=g_WaterScatterColor*scatter_factor;
	color.a=1;
	return color;
}

//--------------------------------------------------------------------------------------
// Water normalmap combine shaders
//--------------------------------------------------------------------------------------

PSIn_Quad WaterNormalmapCombineVS(uint VertexId: SV_VertexID)
{
    PSIn_Quad output;

	output.position = float4(QuadVertices[VertexId],0,1);
    output.texcoord = QuadTexCoordinates[VertexId];
    
    return output;
}

float4 WaterNormalmapCombinePS(PSIn_Quad input) : SV_Target
{
	float4 color;
	color.rgb = (CombineSimplifiedWaterNormal(g_CameraPosition+float3(input.texcoord.x*400.0f-200.0f,0,input.texcoord.y*400.0f-200.0f),0).rgb+float3(1.0f,1.0f,1.0f))*0.5f;
	color.a=0;
	return color;
}

//--------------------------------------------------------------------------------------
// Fullscreen shaders
//--------------------------------------------------------------------------------------

PSIn_Quad FullScreenQuadVS(uint VertexId: SV_VertexID)
{
    PSIn_Quad output;

	output.position = float4(QuadVertices[VertexId],0,1);
    output.texcoord = QuadTexCoordinates[VertexId];
    
    return output;
}

float4 MainToBackBufferPS(PSIn_Quad input) : SV_Target
{
	float4 color;
	color.rgb = g_MainTexture.SampleLevel(SamplerLinearWrap,float2((input.texcoord.x-0.5)/g_MainBufferSizeMultiplier+0.5f,(input.texcoord.y-0.5)/g_MainBufferSizeMultiplier+0.5f),0).rgb;
	color.a=1;
	return color;
}

float RefractionDepthManualResolvePS1(PSIn_Quad input) : SV_Target
{
	return g_RefractionDepthTextureMS1.Load(input.position.xy,0,int2(0,0)).r;
}

float RefractionDepthManualResolvePS2(PSIn_Quad input) : SV_Target
{
	return g_RefractionDepthTextureMS2.Load(input.position.xy,0,int2(0,0)).r;
}

float RefractionDepthManualResolvePS4(PSIn_Quad input) : SV_Target
{
	return g_RefractionDepthTextureMS4.Load(input.position.xy,0,int2(0,0)).r;
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------

technique11 RefractionDepthManualResolve
{
    pass MS1
    {
        SetRasterizerState(NoCullMS);
        SetDepthStencilState(NoDepthStencil, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuadVS()));
        SetHullShader(NULL);
        SetDomainShader(NULL);
        SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RefractionDepthManualResolvePS1()));
    }
    pass MS2
    {
        SetRasterizerState(NoCullMS);
        SetDepthStencilState(NoDepthStencil, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuadVS()));
        SetHullShader(NULL);
        SetDomainShader(NULL);
        SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RefractionDepthManualResolvePS2()));
    }
    pass MS4
    {
        SetRasterizerState(NoCullMS);
        SetDepthStencilState(NoDepthStencil, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuadVS()));
        SetHullShader(NULL);
        SetDomainShader(NULL);
        SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RefractionDepthManualResolvePS4()));
    }
}

technique11 MainToBackBuffer
{
    pass Solid
    {
        SetRasterizerState(NoCullMS);
        SetDepthStencilState(NoDepthStencil, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

        SetVertexShader(CompileShader(vs_4_0, FullScreenQuadVS()));
        SetHullShader(NULL);
        SetDomainShader(NULL);
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, MainToBackBufferPS()));
    }
}

technique11 WaterNormalmapCombine
{
    pass Solid
    {
        SetRasterizerState(NoCullMS);
        SetDepthStencilState(NoDepthStencil, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

        SetVertexShader(CompileShader(vs_4_0, WaterNormalmapCombineVS()));
        SetHullShader(NULL);
        SetDomainShader(NULL);
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, WaterNormalmapCombinePS()));
    }
}


technique11 RenderWater
{
    pass Solid
    {
        SetRasterizerState(CullBackMS);
        SetDepthStencilState(DepthNormal, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

        SetVertexShader(CompileShader(vs_4_0, PassThroughVS()));
        SetHullShader(CompileShader(hs_5_0, PatchHS()));
        SetDomainShader(CompileShader(ds_5_0, WaterPatchDS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, WaterPatchPS()));
    }
    pass Wireframe
    {
        SetRasterizerState(WireframeMS);
        SetDepthStencilState(DepthNormal, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

        SetVertexShader(CompileShader(vs_4_0, PassThroughVS()));
        SetHullShader(CompileShader(hs_5_0, PatchHS()));
        SetDomainShader(CompileShader(ds_5_0, WaterPatchDS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, ColorPS(float4(1.0f, 1.0f, 1.0f, 0.0f))));
    }
}


technique11 Unlit
{
	pass Solid
	{
		SetRasterizerState(CullBackMS);
		SetDepthStencilState(DepthNormal, 0);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader(CompileShader(vs_4_0, PassThroughVS()));
		SetHullShader(CompileShader(hs_5_0, PatchHS()));
		SetDomainShader(CompileShader(ds_5_0, WaterPatchDS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, WaterPatchPS()));
	}
	pass Wireframe
	{
		SetRasterizerState(WireframeMS);
		SetDepthStencilState(DepthNormal, 0);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader(CompileShader(vs_4_0, PassThroughVS()));
		SetHullShader(CompileShader(hs_5_0, PatchHS()));
		SetDomainShader(CompileShader(ds_5_0, WaterPatchDS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, ColorPS(float4(1.0f, 1.0f, 1.0f, 0.0f))));
	}
}

