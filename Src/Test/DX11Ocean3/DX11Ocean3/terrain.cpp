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

#include "stdafx.h"
#include "terrain.h"
#include <time.h>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

extern bool g_RenderWireframe;
extern bool g_RenderCaustics;

int gp_wrap( int a)
{
	if(a<0) return (a+terrain_gridpoints);
	if(a>=terrain_gridpoints) return (a-terrain_gridpoints);
	return a;
}

void CTerrain::Initialize(ID3D11Device* device, ID3DX11Effect * effect)
{
	pEffect = effect;
	pDevice = device;


    const D3D11_INPUT_ELEMENT_DESC TerrainLayout =
        { "PATCH_PARAMETERS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 };

    D3DX11_PASS_DESC passDesc;
    effect->GetTechniqueByName("RenderHeightfield")->GetPassByIndex(0)->GetDesc(&passDesc);
    device->CreateInputLayout( &TerrainLayout, 1, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &heightfield_inputlayout );

    const D3D11_INPUT_ELEMENT_DESC SkyLayout [] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,   0, 16,  D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

    effect->GetTechniqueByName("RenderSky")->GetPassByIndex(0)->GetDesc(&passDesc);
    device->CreateInputLayout( SkyLayout, 
        2, 
        passDesc.pIAInputSignature,
        passDesc.IAInputSignatureSize, 
        &trianglestrip_inputlayout );

	CreateTerrain();
}

void CTerrain::ReCreateBuffers()
{
	D3D11_TEXTURE2D_DESC tex_desc;
	D3D11_SHADER_RESOURCE_VIEW_DESC textureSRV_desc;
	D3D11_DEPTH_STENCIL_VIEW_DESC DSV_desc;

	SAFE_RELEASE(main_color_resource);
	SAFE_RELEASE(main_color_resourceSRV);
	SAFE_RELEASE(main_color_resourceRTV);

	SAFE_RELEASE(main_color_resource_resolved);
	SAFE_RELEASE(main_color_resource_resolvedSRV);

	SAFE_RELEASE(main_depth_resource);
	SAFE_RELEASE(main_depth_resourceDSV);
	SAFE_RELEASE(main_depth_resourceSRV);

	SAFE_RELEASE(reflection_color_resource);
	SAFE_RELEASE(reflection_color_resourceSRV);
	SAFE_RELEASE(reflection_color_resourceRTV);
	SAFE_RELEASE(refraction_color_resource);
	SAFE_RELEASE(refraction_color_resourceSRV);
	SAFE_RELEASE(refraction_color_resourceRTV);

	SAFE_RELEASE(reflection_depth_resource);
	SAFE_RELEASE(reflection_depth_resourceDSV);
	SAFE_RELEASE(refraction_depth_resource);
	SAFE_RELEASE(refraction_depth_resourceSRV);
	SAFE_RELEASE(refraction_depth_resourceRTV);

	SAFE_RELEASE(shadowmap_resource);
	SAFE_RELEASE(shadowmap_resourceDSV);
	SAFE_RELEASE(shadowmap_resourceSRV);

	SAFE_RELEASE(water_normalmap_resource);
	SAFE_RELEASE(water_normalmap_resourceSRV);
	SAFE_RELEASE(water_normalmap_resourceRTV);

	// recreating main color buffer

	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	ZeroMemory(&tex_desc,sizeof(tex_desc));

	tex_desc.Width              = (UINT)(BackbufferWidth*main_buffer_size_multiplier);
    tex_desc.Height             = (UINT)(BackbufferHeight*main_buffer_size_multiplier);
    tex_desc.MipLevels          = 1;
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.SampleDesc.Count   = MultiSampleCount;
    tex_desc.SampleDesc.Quality = MultiSampleQuality;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = 0;

    textureSRV_desc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureSRV_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	textureSRV_desc.Texture2D.MipLevels = tex_desc.MipLevels;
	textureSRV_desc.Texture2D.MostDetailedMip = 0;

	pDevice->CreateTexture2D         ( &tex_desc, NULL, &main_color_resource );
    pDevice->CreateShaderResourceView( main_color_resource, &textureSRV_desc, &main_color_resourceSRV );
    pDevice->CreateRenderTargetView  ( main_color_resource, NULL, &main_color_resourceRTV );


	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	ZeroMemory(&tex_desc,sizeof(tex_desc));

	tex_desc.Width              = (UINT)(BackbufferWidth*main_buffer_size_multiplier);
    tex_desc.Height             = (UINT)(BackbufferHeight*main_buffer_size_multiplier);
    tex_desc.MipLevels          = 1;
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.SampleDesc.Count   = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = 0;

    textureSRV_desc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureSRV_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureSRV_desc.Texture2D.MipLevels = tex_desc.MipLevels;
	textureSRV_desc.Texture2D.MostDetailedMip = 0;

	pDevice->CreateTexture2D         ( &tex_desc, NULL, &main_color_resource_resolved );
    pDevice->CreateShaderResourceView( main_color_resource_resolved, &textureSRV_desc, &main_color_resource_resolvedSRV );

	// recreating main depth buffer

	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	ZeroMemory(&tex_desc,sizeof(tex_desc));

	tex_desc.Width              = (UINT)(BackbufferWidth*main_buffer_size_multiplier);
    tex_desc.Height             = (UINT)(BackbufferHeight*main_buffer_size_multiplier);
    tex_desc.MipLevels          = 1;
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R32_TYPELESS;
	tex_desc.SampleDesc.Count   = MultiSampleCount;
    tex_desc.SampleDesc.Quality = MultiSampleQuality;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = 0;
	
	DSV_desc.Format  = DXGI_FORMAT_D32_FLOAT;
	DSV_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	DSV_desc.Flags = 0;
	DSV_desc.Texture2D.MipSlice = 0;

    textureSRV_desc.Format                    = DXGI_FORMAT_R32_FLOAT;
    textureSRV_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	textureSRV_desc.Texture2D.MipLevels		  = 1;
	textureSRV_desc.Texture2D.MostDetailedMip = 0;

	pDevice->CreateTexture2D( &tex_desc, NULL, &main_depth_resource );
 	pDevice->CreateDepthStencilView(main_depth_resource, &DSV_desc, &main_depth_resourceDSV );
	pDevice->CreateShaderResourceView( main_depth_resource, &textureSRV_desc, &main_depth_resourceSRV );

	// recreating reflection and refraction color buffers

	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	ZeroMemory(&tex_desc,sizeof(tex_desc));

	tex_desc.Width              = (UINT)(BackbufferWidth*reflection_buffer_size_multiplier);
    tex_desc.Height             = (UINT)(BackbufferHeight*reflection_buffer_size_multiplier);
    tex_desc.MipLevels          = (UINT)max(1,log(max((float)tex_desc.Width,(float)tex_desc.Height))/(float)log(2.0f));
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex_desc.SampleDesc.Count   = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    textureSRV_desc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureSRV_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureSRV_desc.Texture2D.MipLevels = tex_desc.MipLevels;
	textureSRV_desc.Texture2D.MostDetailedMip = 0;

	pDevice->CreateTexture2D         ( &tex_desc, NULL, &reflection_color_resource );
    pDevice->CreateShaderResourceView( reflection_color_resource, &textureSRV_desc, &reflection_color_resourceSRV );
    pDevice->CreateRenderTargetView  ( reflection_color_resource, NULL, &reflection_color_resourceRTV );


	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	ZeroMemory(&tex_desc,sizeof(tex_desc));

	tex_desc.Width              = (UINT)(BackbufferWidth*refraction_buffer_size_multiplier);
    tex_desc.Height             = (UINT)(BackbufferHeight*refraction_buffer_size_multiplier);
    tex_desc.MipLevels          = (UINT)max(1,log(max((float)tex_desc.Width,(float)tex_desc.Height))/(float)log(2.0f));
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex_desc.SampleDesc.Count   = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    textureSRV_desc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureSRV_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureSRV_desc.Texture2D.MipLevels = tex_desc.MipLevels;
	textureSRV_desc.Texture2D.MostDetailedMip = 0;

	pDevice->CreateTexture2D         ( &tex_desc, NULL, &refraction_color_resource );
    pDevice->CreateShaderResourceView( refraction_color_resource, &textureSRV_desc, &refraction_color_resourceSRV );
    pDevice->CreateRenderTargetView  ( refraction_color_resource, NULL, &refraction_color_resourceRTV );

	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	ZeroMemory(&tex_desc,sizeof(tex_desc));

	// recreating reflection and refraction depth buffers

	tex_desc.Width              = (UINT)(BackbufferWidth*reflection_buffer_size_multiplier);
    tex_desc.Height             = (UINT)(BackbufferHeight*reflection_buffer_size_multiplier);
    tex_desc.MipLevels          = 1;
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R32_TYPELESS;
    tex_desc.SampleDesc.Count   = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = 0;
	
	DSV_desc.Format  = DXGI_FORMAT_D32_FLOAT;
	DSV_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSV_desc.Flags = 0;
	DSV_desc.Texture2D.MipSlice = 0;

	pDevice->CreateTexture2D( &tex_desc, NULL, &reflection_depth_resource );
 	pDevice->CreateDepthStencilView(reflection_depth_resource, &DSV_desc, &reflection_depth_resourceDSV );


	tex_desc.Width              = (UINT)(BackbufferWidth*refraction_buffer_size_multiplier);
    tex_desc.Height             = (UINT)(BackbufferHeight*refraction_buffer_size_multiplier);
    tex_desc.MipLevels          = 1;
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R32_FLOAT;
    tex_desc.SampleDesc.Count   = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = 0;
	
	textureSRV_desc.Format                    = DXGI_FORMAT_R32_FLOAT;
    textureSRV_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    textureSRV_desc.Texture2D.MipLevels       = 1;
    textureSRV_desc.Texture2D.MostDetailedMip = 0;

	pDevice->CreateTexture2D( &tex_desc, NULL, &refraction_depth_resource );
 	pDevice->CreateRenderTargetView  (refraction_depth_resource, NULL, &refraction_depth_resourceRTV );
	pDevice->CreateShaderResourceView(refraction_depth_resource, &textureSRV_desc, &refraction_depth_resourceSRV );

	// recreating shadowmap resource
	tex_desc.Width              = shadowmap_resource_buffer_size_xy;
    tex_desc.Height             = shadowmap_resource_buffer_size_xy;
    tex_desc.MipLevels          = 1;
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R32_TYPELESS;
    tex_desc.SampleDesc.Count   = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = 0;
	
	DSV_desc.Format  = DXGI_FORMAT_D32_FLOAT;
	DSV_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSV_desc.Flags = 0;
	DSV_desc.Texture2D.MipSlice=0;

	textureSRV_desc.Format                    = DXGI_FORMAT_R32_FLOAT;
    textureSRV_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    textureSRV_desc.Texture2D.MipLevels       = 1;
    textureSRV_desc.Texture2D.MostDetailedMip = 0;

	pDevice->CreateTexture2D( &tex_desc, NULL, &shadowmap_resource);	
	pDevice->CreateShaderResourceView( shadowmap_resource, &textureSRV_desc, &shadowmap_resourceSRV );
	pDevice->CreateDepthStencilView(shadowmap_resource, &DSV_desc, &shadowmap_resourceDSV );

	// recreating water normalmap buffer

	tex_desc.Width              = water_normalmap_resource_buffer_size_xy;
    tex_desc.Height             = water_normalmap_resource_buffer_size_xy;
    tex_desc.MipLevels          = 1;
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex_desc.SampleDesc.Count   = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = 0;

    textureSRV_desc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureSRV_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureSRV_desc.Texture2D.MipLevels = tex_desc.MipLevels;
	textureSRV_desc.Texture2D.MostDetailedMip = 0;

	pDevice->CreateTexture2D         ( &tex_desc, NULL, &water_normalmap_resource );
    pDevice->CreateShaderResourceView( water_normalmap_resource, &textureSRV_desc, &water_normalmap_resourceSRV );
    pDevice->CreateRenderTargetView  ( water_normalmap_resource, NULL, &water_normalmap_resourceRTV );

}

void CTerrain::DeInitialize()
{
	SAFE_RELEASE(heightmap_texture);
	SAFE_RELEASE(heightmap_textureSRV);

	SAFE_RELEASE(rock_bump_texture);
	SAFE_RELEASE(rock_bump_textureSRV);
	
	SAFE_RELEASE(rock_microbump_texture);
	SAFE_RELEASE(rock_microbump_textureSRV);

	SAFE_RELEASE(rock_diffuse_texture);
	SAFE_RELEASE(rock_diffuse_textureSRV);


	SAFE_RELEASE(sand_bump_texture);
	SAFE_RELEASE(sand_bump_textureSRV);

	SAFE_RELEASE(sand_microbump_texture);
	SAFE_RELEASE(sand_microbump_textureSRV);

	SAFE_RELEASE(sand_diffuse_texture);
	SAFE_RELEASE(sand_diffuse_textureSRV);

	SAFE_RELEASE(slope_diffuse_texture);
	SAFE_RELEASE(slope_diffuse_textureSRV);

	SAFE_RELEASE(grass_diffuse_texture);
	SAFE_RELEASE(grass_diffuse_textureSRV);

	SAFE_RELEASE(layerdef_texture);
	SAFE_RELEASE(layerdef_textureSRV);

	SAFE_RELEASE(water_bump_texture);
	SAFE_RELEASE(water_bump_textureSRV);

	SAFE_RELEASE(depthmap_texture);
	SAFE_RELEASE(depthmap_textureSRV);

	SAFE_RELEASE(sky_texture);
	SAFE_RELEASE(sky_textureSRV);



	SAFE_RELEASE(main_color_resource);
	SAFE_RELEASE(main_color_resourceSRV);
	SAFE_RELEASE(main_color_resourceRTV);

	SAFE_RELEASE(main_color_resource_resolved);
	SAFE_RELEASE(main_color_resource_resolvedSRV);

	SAFE_RELEASE(main_depth_resource);
	SAFE_RELEASE(main_depth_resourceDSV);
	SAFE_RELEASE(main_depth_resourceSRV);

	SAFE_RELEASE(reflection_color_resource);
	SAFE_RELEASE(reflection_color_resourceSRV);
	SAFE_RELEASE(reflection_color_resourceRTV);
	SAFE_RELEASE(refraction_color_resource);
	SAFE_RELEASE(refraction_color_resourceSRV);
	SAFE_RELEASE(refraction_color_resourceRTV);

	SAFE_RELEASE(reflection_depth_resource);
	SAFE_RELEASE(reflection_depth_resourceDSV);
	SAFE_RELEASE(refraction_depth_resource);
	SAFE_RELEASE(refraction_depth_resourceRTV);
	SAFE_RELEASE(refraction_depth_resourceSRV);

	SAFE_RELEASE(shadowmap_resource);
	SAFE_RELEASE(shadowmap_resourceDSV);
	SAFE_RELEASE(shadowmap_resourceSRV);

	SAFE_RELEASE(sky_vertexbuffer);
	SAFE_RELEASE(trianglestrip_inputlayout);

	SAFE_RELEASE(heightfield_vertexbuffer);
	SAFE_RELEASE(heightfield_inputlayout);

	SAFE_RELEASE(water_normalmap_resource);
	SAFE_RELEASE(water_normalmap_resourceSRV);
	SAFE_RELEASE(water_normalmap_resourceRTV);

}
void CTerrain::CreateTerrain()
{
	int i,j,k,l;
	float x,z;
	int ix,iz;
	float * backterrain;
	Vector3 vec1,vec2,vec3;
	int currentstep=terrain_gridpoints;
	float mv,rm;
	float offset=0,yscale=0,maxheight=0,minheight=0;

	float *height_linear_array;
	float *patches_rawdata;
	HRESULT result;
	D3D11_SUBRESOURCE_DATA subresource_data;
	D3D11_TEXTURE2D_DESC tex_desc;
	D3D11_SHADER_RESOURCE_VIEW_DESC textureSRV_desc; 

	backterrain = (float *) malloc((terrain_gridpoints+1)*(terrain_gridpoints+1)*sizeof(float));
	rm=terrain_fractalinitialvalue;
	backterrain[0]=0;
	backterrain[0+terrain_gridpoints*terrain_gridpoints]=0;
	backterrain[terrain_gridpoints]=0;
	backterrain[terrain_gridpoints+terrain_gridpoints*terrain_gridpoints]=0;
    currentstep=terrain_gridpoints;
	srand(12);
	
	// generating fractal terrain using square-diamond method
	while (currentstep>1)
	{
		//square step;
		i=0;
		j=0;


		while (i<terrain_gridpoints)
		{
			j=0;
			while (j<terrain_gridpoints)
			{
				
				mv=backterrain[i+terrain_gridpoints*j];
				mv+=backterrain[(i+currentstep)+terrain_gridpoints*j];
				mv+=backterrain[(i+currentstep)+terrain_gridpoints*(j+currentstep)];
				mv+=backterrain[i+terrain_gridpoints*(j+currentstep)];
				mv/=4.0;
				backterrain[i+currentstep/2+terrain_gridpoints*(j+currentstep/2)]=(float)(mv+rm*((rand()%1000)/1000.0f-0.5f));
				j+=currentstep;
			}
		i+=currentstep;
		}

		//diamond step;
		i=0;
		j=0;

		while (i<terrain_gridpoints)
		{
			j=0;
			while (j<terrain_gridpoints)
			{

				mv=0;
				mv=backterrain[i+terrain_gridpoints*j];
				mv+=backterrain[(i+currentstep)+terrain_gridpoints*j];
				mv+=backterrain[(i+currentstep/2)+terrain_gridpoints*(j+currentstep/2)];
				mv+=backterrain[i+currentstep/2+terrain_gridpoints*gp_wrap(j-currentstep/2)];
				mv/=4;
				backterrain[i+currentstep/2+terrain_gridpoints*j]=(float)(mv+rm*((rand()%1000)/1000.0f-0.5f));

				mv=0;
				mv=backterrain[i+terrain_gridpoints*j];
				mv+=backterrain[i+terrain_gridpoints*(j+currentstep)];
				mv+=backterrain[(i+currentstep/2)+terrain_gridpoints*(j+currentstep/2)];
				mv+=backterrain[gp_wrap(i-currentstep/2)+terrain_gridpoints*(j+currentstep/2)];
				mv/=4;
				backterrain[i+terrain_gridpoints*(j+currentstep/2)]=(float)(mv+rm*((rand()%1000)/1000.0f-0.5f));

				mv=0;
				mv=backterrain[i+currentstep+terrain_gridpoints*j];
				mv+=backterrain[i+currentstep+terrain_gridpoints*(j+currentstep)];
				mv+=backterrain[(i+currentstep/2)+terrain_gridpoints*(j+currentstep/2)];
				mv+=backterrain[gp_wrap(i+currentstep/2+currentstep)+terrain_gridpoints*(j+currentstep/2)];
				mv/=4;
				backterrain[i+currentstep+terrain_gridpoints*(j+currentstep/2)]=(float)(mv+rm*((rand()%1000)/1000.0f-0.5f));

				mv=0;
				mv=backterrain[i+currentstep+terrain_gridpoints*(j+currentstep)];
				mv+=backterrain[i+terrain_gridpoints*(j+currentstep)];
				mv+=backterrain[(i+currentstep/2)+terrain_gridpoints*(j+currentstep/2)];
				mv+=backterrain[i+currentstep/2+terrain_gridpoints*gp_wrap(j+currentstep/2+currentstep)];
				mv/=4;
				backterrain[i+currentstep/2+terrain_gridpoints*(j+currentstep)]=(float)(mv+rm*((rand()%1000)/1000.0f-0.5f));
				j+=currentstep;
			}
			i+=currentstep;
		}
		//changing current step;
		currentstep/=2;
        rm*=terrain_fractalfactor;
	}	

	// scaling to minheight..maxheight range
	for (i=0;i<terrain_gridpoints+1;i++)
		for (j=0;j<terrain_gridpoints+1;j++)
		{
			height[i][j]=backterrain[i+terrain_gridpoints*j];
		}
	maxheight=height[0][0];
	minheight=height[0][0];
	for(i=0;i<terrain_gridpoints+1;i++)
		for(j=0;j<terrain_gridpoints+1;j++)
		{
			if(height[i][j]>maxheight) maxheight=height[i][j];
			if(height[i][j]<minheight) minheight=height[i][j];
		}
	offset=minheight-terrain_minheight;
	yscale=(terrain_maxheight-terrain_minheight)/(maxheight-minheight);

	for(i=0;i<terrain_gridpoints+1;i++)
		for(j=0;j<terrain_gridpoints+1;j++)
		{
			height[i][j]-=minheight;
			height[i][j]*=yscale;
			height[i][j]+=terrain_minheight;
		}

	// moving down edges of heightmap	
	for (i=0;i<terrain_gridpoints+1;i++)
		for (j=0;j<terrain_gridpoints+1;j++)
		{
			mv=(float)((i-terrain_gridpoints/2.0f)*(i-terrain_gridpoints/2.0f)+(j-terrain_gridpoints/2.0f)*(j-terrain_gridpoints/2.0f));
			rm=(float)((terrain_gridpoints*0.8f)*(terrain_gridpoints*0.8f)/4.0f);
			if(mv>rm)
			{
				height[i][j]-=((mv-rm)/1000.0f)*terrain_geometry_scale;
			}
			if(height[i][j]<terrain_minheight)
			{
				height[i][j]=terrain_minheight;
			}
		}	


	// terrain banks
	for(k=0;k<10;k++)
	{	
		for(i=0;i<terrain_gridpoints+1;i++)
			for(j=0;j<terrain_gridpoints+1;j++)
			{
				mv=height[i][j];
				if((mv)>0.02f) 
				{
					mv-=0.02f;
				}
				if(mv<-0.02f) 
				{
					mv+=0.02f;
				}
				height[i][j]=mv;
			}
	}

	// smoothing 
	for(k=0;k<terrain_smoothsteps;k++)
	{	
		for(i=0;i<terrain_gridpoints+1;i++)
			for(j=0;j<terrain_gridpoints+1;j++)
			{

				vec1.x=2*terrain_geometry_scale;
				vec1.y=terrain_geometry_scale*(height[gp_wrap(i+1)][j]-height[gp_wrap(i-1)][j]);
				vec1.z=0;
				vec2.x=0;
				vec2.y=-terrain_geometry_scale*(height[i][gp_wrap(j+1)]-height[i][gp_wrap(j-1)]);
				vec2.z=-2*terrain_geometry_scale;

				//D3DXVec3Cross(&vec3,&vec1,&vec2);
				//D3DXVec3Normalize(&vec3,&vec3);
				vec3 = vec1.CrossProduct(vec2);
				vec3.Normalize();
				
				if(((vec3.y>terrain_rockfactor)||(height[i][j]<1.2f))) 
				{
					rm=terrain_smoothfactor1;
					mv=height[i][j]*(1.0f-rm) +rm*0.25f*(height[gp_wrap(i-1)][j]+height[i][gp_wrap(j-1)]+height[gp_wrap(i+1)][j]+height[i][gp_wrap(j+1)]);
					backterrain[i+terrain_gridpoints*j]=mv;
				}
				else
				{
					rm=terrain_smoothfactor2;
					mv=height[i][j]*(1.0f-rm) +rm*0.25f*(height[gp_wrap(i-1)][j]+height[i][gp_wrap(j-1)]+height[gp_wrap(i+1)][j]+height[i][gp_wrap(j+1)]);
					backterrain[i+terrain_gridpoints*j]=mv;
				}

			}
			for (i=0;i<terrain_gridpoints+1;i++)
				for (j=0;j<terrain_gridpoints+1;j++)
				{
					height[i][j]=(backterrain[i+terrain_gridpoints*j]);
				}
	}
	for(i=0;i<terrain_gridpoints+1;i++)
		for(j=0;j<terrain_gridpoints+1;j++)
		{
			rm=0.5f;
			mv=height[i][j]*(1.0f-rm) +rm*0.25f*(height[gp_wrap(i-1)][j]+height[i][gp_wrap(j-1)]+height[gp_wrap(i+1)][j]+height[i][gp_wrap(j+1)]);
			backterrain[i+terrain_gridpoints*j]=mv;
		}
	for (i=0;i<terrain_gridpoints+1;i++)
		for (j=0;j<terrain_gridpoints+1;j++)
		{
			height[i][j]=(backterrain[i+terrain_gridpoints*j]);
		}


	free(backterrain);

		//calculating normals
		for (i=0;i<terrain_gridpoints+1;i++)
			for (j=0;j<terrain_gridpoints+1;j++)
			{
				vec1.x=2*terrain_geometry_scale;
				vec1.y=terrain_geometry_scale*(height[gp_wrap(i+1)][j]-height[gp_wrap(i-1)][j]);
				vec1.z=0;
				vec2.x=0;
				vec2.y=-terrain_geometry_scale*(height[i][gp_wrap(j+1)]-height[i][gp_wrap(j-1)]);
				vec2.z=-2*terrain_geometry_scale;
				//D3DXVec3Cross(&normal[i][j],&vec1,&vec2);
				//D3DXVec3Normalize(&normal[i][j],&normal[i][j]);

				normal[i][j] = vec1.CrossProduct(vec2);
				normal[i][j].Normalize();
			}


	// buiding layerdef 
	byte* temp_layerdef_map_texture_pixels=(byte *)malloc(terrain_layerdef_map_texture_size*terrain_layerdef_map_texture_size*4);
	byte* layerdef_map_texture_pixels=(byte *)malloc(terrain_layerdef_map_texture_size*terrain_layerdef_map_texture_size*4);
	for(i=0;i<terrain_layerdef_map_texture_size;i++)
		for(j=0;j<terrain_layerdef_map_texture_size;j++)
		{
			x=(float)(terrain_gridpoints)*((float)i/(float)terrain_layerdef_map_texture_size);
			z=(float)(terrain_gridpoints)*((float)j/(float)terrain_layerdef_map_texture_size);
			ix=(int)floor(x);
			iz=(int)floor(z);
			rm=bilinear_interpolation(x-ix,z-iz,height[ix][iz],height[ix+1][iz],height[ix+1][iz+1],height[ix][iz+1])*terrain_geometry_scale;
			
			temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+0]=0;
			temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+1]=0;
			temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+2]=0;
			temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+3]=0;

			if((rm>terrain_height_underwater_start)&&(rm<=terrain_height_underwater_end))
			{
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+0]=255;
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+1]=0;
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+2]=0;
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+3]=0;
			}

			if((rm>terrain_height_sand_start)&&(rm<=terrain_height_sand_end))
			{
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+0]=0;
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+1]=255;
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+2]=0;
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+3]=0;
			}

			if((rm>terrain_height_grass_start)&&(rm<=terrain_height_grass_end))
			{
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+0]=0;
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+1]=0;
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+2]=255;
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+3]=0;
			}

			//mv = bilinear_interpolation(x-ix,z-iz,normal[ix][iz][1],normal[ix+1][iz][1],normal[ix+1][iz+1][1],normal[ix][iz+1][1]);
			mv = bilinear_interpolation(x - ix, z - iz, normal[ix][iz].y, normal[ix + 1][iz].y, normal[ix + 1][iz + 1].y, normal[ix][iz + 1].y);

			if((mv<terrain_slope_grass_start)&&(rm>terrain_height_sand_end))
			{
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+0]=0;
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+1]=0;
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+2]=0;
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+3]=0;
			}

			if((mv<terrain_slope_rocks_start)&&(rm>terrain_height_rocks_start))
			{
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+0]=0;
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+1]=0;
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+2]=0;
				temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+3]=255;
			}

		}
	for(i=0;i<terrain_layerdef_map_texture_size;i++)
		for(j=0;j<terrain_layerdef_map_texture_size;j++)
		{
			layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+0]=temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+0];
			layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+1]=temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+1];
			layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+2]=temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+2];
			layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+3]=temp_layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+3];
		}


	for(i=2;i<terrain_layerdef_map_texture_size-2;i++)
		for(j=2;j<terrain_layerdef_map_texture_size-2;j++)
		{
			int n1=0;
			int n2=0;
			int n3=0;
			int n4=0;
			for(k=-2;k<3;k++)
				for(l=-2;l<3;l++)
					{
							n1+=temp_layerdef_map_texture_pixels[((j+k)*terrain_layerdef_map_texture_size+i+l)*4+0];
							n2+=temp_layerdef_map_texture_pixels[((j+k)*terrain_layerdef_map_texture_size+i+l)*4+1];
							n3+=temp_layerdef_map_texture_pixels[((j+k)*terrain_layerdef_map_texture_size+i+l)*4+2];
							n4+=temp_layerdef_map_texture_pixels[((j+k)*terrain_layerdef_map_texture_size+i+l)*4+3];
					}
			layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+0]=(byte)(n1/25);
			layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+1]=(byte)(n2/25);
			layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+2]=(byte)(n3/25);
			layerdef_map_texture_pixels[(j*terrain_layerdef_map_texture_size+i)*4+3]=(byte)(n4/25);
		}

	// putting the generated data to textures

	subresource_data.pSysMem = layerdef_map_texture_pixels;
	subresource_data.SysMemPitch = terrain_layerdef_map_texture_size*4;
	subresource_data.SysMemSlicePitch = 0;

	tex_desc.Width = terrain_layerdef_map_texture_size;
	tex_desc.Height = terrain_layerdef_map_texture_size;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.SampleDesc.Count = 1; 
	tex_desc.SampleDesc.Quality = 0; 
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags = 0;
	tex_desc.MiscFlags = 0;
	result=pDevice->CreateTexture2D(&tex_desc,&subresource_data,&layerdef_texture);

	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	textureSRV_desc.Format=tex_desc.Format;
	textureSRV_desc.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
	textureSRV_desc.Texture2D.MipLevels=tex_desc.MipLevels;
	textureSRV_desc.Texture2D.MostDetailedMip=0;
	pDevice->CreateShaderResourceView(layerdef_texture,&textureSRV_desc,&layerdef_textureSRV);

	free(temp_layerdef_map_texture_pixels);
	free(layerdef_map_texture_pixels);

	height_linear_array = new float [terrain_gridpoints*terrain_gridpoints*4];
	patches_rawdata = new float [terrain_numpatches_1d*terrain_numpatches_1d*4];

	for(int i=0;i<terrain_gridpoints;i++)
		for(int j=0; j<terrain_gridpoints;j++)
		{
			height_linear_array[(i+j*terrain_gridpoints)*4+0]=normal[i][j].x;
			height_linear_array[(i+j*terrain_gridpoints)*4+1]=normal[i][j].y;
			height_linear_array[(i+j*terrain_gridpoints)*4+2]=normal[i][j].z;
			height_linear_array[(i+j*terrain_gridpoints)*4+3]=height[i][j];
		}
	subresource_data.pSysMem = height_linear_array;
	subresource_data.SysMemPitch = terrain_gridpoints*4*sizeof(float);
	subresource_data.SysMemSlicePitch = 0;

	tex_desc.Width = terrain_gridpoints;
	tex_desc.Height = terrain_gridpoints;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex_desc.SampleDesc.Count = 1; 
	tex_desc.SampleDesc.Quality = 0; 
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags = 0;
	tex_desc.MiscFlags = 0;
	result=pDevice->CreateTexture2D(&tex_desc,&subresource_data,&heightmap_texture);

	free(height_linear_array);

	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	textureSRV_desc.Format=tex_desc.Format;
	textureSRV_desc.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
	textureSRV_desc.Texture2D.MipLevels=tex_desc.MipLevels;
	pDevice->CreateShaderResourceView(heightmap_texture,&textureSRV_desc,&heightmap_textureSRV);

	//building depthmap
	byte * depth_shadow_map_texture_pixels=(byte *)malloc(terrain_depth_shadow_map_texture_size*terrain_depth_shadow_map_texture_size*4);
	for(i=0;i<terrain_depth_shadow_map_texture_size;i++)
		for(j=0;j<terrain_depth_shadow_map_texture_size;j++)
		{
			x=(float)(terrain_gridpoints)*((float)i/(float)terrain_depth_shadow_map_texture_size);
			z=(float)(terrain_gridpoints)*((float)j/(float)terrain_depth_shadow_map_texture_size);
			ix=(int)floor(x);
			iz=(int)floor(z);
			rm=bilinear_interpolation(x-ix,z-iz,height[ix][iz],height[ix+1][iz],height[ix+1][iz+1],height[ix][iz+1])*terrain_geometry_scale;
			
			if(rm>0)
			{
				depth_shadow_map_texture_pixels[(j*terrain_depth_shadow_map_texture_size+i)*4+0]=0;
				depth_shadow_map_texture_pixels[(j*terrain_depth_shadow_map_texture_size+i)*4+1]=0;
				depth_shadow_map_texture_pixels[(j*terrain_depth_shadow_map_texture_size+i)*4+2]=0;
			}
			else
			{
				float no=(1.0f*255.0f*(rm/(terrain_minheight*terrain_geometry_scale)))-1.0f;
				if(no>255) no=255;
				if(no<0) no=0;
				depth_shadow_map_texture_pixels[(j*terrain_depth_shadow_map_texture_size+i)*4+0]=(byte)no;
				
				no=(10.0f*255.0f*(rm/(terrain_minheight*terrain_geometry_scale)))-40.0f;
				if(no>255) no=255;
				if(no<0) no=0;
				depth_shadow_map_texture_pixels[(j*terrain_depth_shadow_map_texture_size+i)*4+1]=(byte)no;

				no=(100.0f*255.0f*(rm/(terrain_minheight*terrain_geometry_scale)))-300.0f;
				if(no>255) no=255;
				if(no<0) no=0;
				depth_shadow_map_texture_pixels[(j*terrain_depth_shadow_map_texture_size+i)*4+2]=(byte)no;
			}
			depth_shadow_map_texture_pixels[(j*terrain_depth_shadow_map_texture_size+i)*4+3]=0;
		}
		
	subresource_data.pSysMem = depth_shadow_map_texture_pixels;
	subresource_data.SysMemPitch = terrain_depth_shadow_map_texture_size*4;
	subresource_data.SysMemSlicePitch = 0;

	tex_desc.Width = terrain_depth_shadow_map_texture_size;
	tex_desc.Height = terrain_depth_shadow_map_texture_size;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.SampleDesc.Count = 1; 
	tex_desc.SampleDesc.Quality = 0; 
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags = 0;
	tex_desc.MiscFlags = 0;
	result=pDevice->CreateTexture2D(&tex_desc,&subresource_data,&depthmap_texture);

	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	textureSRV_desc.Format=tex_desc.Format;
	textureSRV_desc.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
	textureSRV_desc.Texture2D.MipLevels=tex_desc.MipLevels;
	pDevice->CreateShaderResourceView(depthmap_texture,&textureSRV_desc,&depthmap_textureSRV);

	free(depth_shadow_map_texture_pixels);

	// creating terrain vertex buffer
	for(int i=0;i<terrain_numpatches_1d;i++)
		for(int j=0;j<terrain_numpatches_1d;j++)
		{
			patches_rawdata[(i+j*terrain_numpatches_1d)*4+0]=i*terrain_geometry_scale*terrain_gridpoints/terrain_numpatches_1d;
			patches_rawdata[(i+j*terrain_numpatches_1d)*4+1]=j*terrain_geometry_scale*terrain_gridpoints/terrain_numpatches_1d;
			patches_rawdata[(i+j*terrain_numpatches_1d)*4+2]=terrain_geometry_scale*terrain_gridpoints/terrain_numpatches_1d;
			patches_rawdata[(i+j*terrain_numpatches_1d)*4+3]=terrain_geometry_scale*terrain_gridpoints/terrain_numpatches_1d;
		}

	D3D11_BUFFER_DESC buf_desc;
	memset(&buf_desc,0,sizeof(buf_desc));

	buf_desc.ByteWidth = terrain_numpatches_1d*terrain_numpatches_1d*4*sizeof(float);
	buf_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buf_desc.Usage = D3D11_USAGE_DEFAULT;

	subresource_data.pSysMem=patches_rawdata;
	subresource_data.SysMemPitch=0;
	subresource_data.SysMemSlicePitch=0;

	result=pDevice->CreateBuffer(&buf_desc,&subresource_data,&heightfield_vertexbuffer);
	free (patches_rawdata);

	// creating sky vertex buffer
	float *sky_vertexdata;
	int floatnum;
	sky_vertexdata = new float [sky_gridpoints*(sky_gridpoints+2)*2*6];

	for(int j=0;j<sky_gridpoints;j++)
	{
		
		i=0;
		floatnum=(j*(sky_gridpoints+2)*2)*6;
		sky_vertexdata[floatnum+0]=terrain_gridpoints*terrain_geometry_scale*0.5f+4000.0f*cos(2.0f*PI*(float)i/(float)sky_gridpoints)*cos(-0.5f*PI+PI*(float)j/(float)sky_gridpoints);
		sky_vertexdata[floatnum+1]=4000.0f*sin(-0.5f*PI+PI*(float)(j)/(float)sky_gridpoints);
		sky_vertexdata[floatnum+2]=terrain_gridpoints*terrain_geometry_scale*0.5f+4000.0f*sin(2.0f*PI*(float)i/(float)sky_gridpoints)*cos(-0.5f*PI+PI*(float)j/(float)sky_gridpoints);
		sky_vertexdata[floatnum+3]=1;
		sky_vertexdata[floatnum+4]=(sky_texture_angle+(float)i/(float)sky_gridpoints);
		sky_vertexdata[floatnum+5]=2.0f-2.0f*(float)j/(float)sky_gridpoints;
		floatnum+=6;
		for(i=0;i<sky_gridpoints+1;i++)
		{
			sky_vertexdata[floatnum+0]=terrain_gridpoints*terrain_geometry_scale*0.5f+4000.0f*cos(2.0f*PI*(float)i/(float)sky_gridpoints)*cos(-0.5f*PI+PI*(float)j/(float)sky_gridpoints);
			sky_vertexdata[floatnum+1]=4000.0f*sin(-0.5f*PI+PI*(float)(j)/(float)sky_gridpoints);
			sky_vertexdata[floatnum+2]=terrain_gridpoints*terrain_geometry_scale*0.5f+4000.0f*sin(2.0f*PI*(float)i/(float)sky_gridpoints)*cos(-0.5f*PI+PI*(float)j/(float)sky_gridpoints);
			sky_vertexdata[floatnum+3]=1;
			sky_vertexdata[floatnum+4]=(sky_texture_angle+(float)i/(float)sky_gridpoints);
			sky_vertexdata[floatnum+5]=2.0f-2.0f*(float)j/(float)sky_gridpoints;
			floatnum+=6;
			sky_vertexdata[floatnum+0]=terrain_gridpoints*terrain_geometry_scale*0.5f+4000.0f*cos(2.0f*PI*(float)i/(float)sky_gridpoints)*cos(-0.5f*PI+PI*(float)(j+1)/(float)sky_gridpoints);
			sky_vertexdata[floatnum+1]=4000.0f*sin(-0.5f*PI+PI*(float)(j+1)/(float)sky_gridpoints);
			sky_vertexdata[floatnum+2]=terrain_gridpoints*terrain_geometry_scale*0.5f+4000.0f*sin(2.0f*PI*(float)i/(float)sky_gridpoints)*cos(-0.5f*PI+PI*(float)(j+1)/(float)sky_gridpoints);
			sky_vertexdata[floatnum+3]=1;
			sky_vertexdata[floatnum+4]=(sky_texture_angle+(float)i/(float)sky_gridpoints);
			sky_vertexdata[floatnum+5]=2.0f-2.0f*(float)(j+1)/(float)sky_gridpoints;
			floatnum+=6;
		}
		i=sky_gridpoints;
		sky_vertexdata[floatnum+0]=terrain_gridpoints*terrain_geometry_scale*0.5f+4000.0f*cos(2.0f*PI*(float)i/(float)sky_gridpoints)*cos(-0.5f*PI+PI*(float)(j+1)/(float)sky_gridpoints);
		sky_vertexdata[floatnum+1]=4000.0f*sin(-0.5f*PI+PI*(float)(j+1)/(float)sky_gridpoints);
		sky_vertexdata[floatnum+2]=terrain_gridpoints*terrain_geometry_scale*0.5f+4000.0f*sin(2.0f*PI*(float)i/(float)sky_gridpoints)*cos(-0.5f*PI+PI*(float)(j+1)/(float)sky_gridpoints);
		sky_vertexdata[floatnum+3]=1;
		sky_vertexdata[floatnum+4]=(sky_texture_angle+(float)i/(float)sky_gridpoints);
		sky_vertexdata[floatnum+5]=2.0f-2.0f*(float)(j+1)/(float)sky_gridpoints;
		floatnum+=6;
	}

	memset(&buf_desc,0,sizeof(buf_desc));

	buf_desc.ByteWidth = sky_gridpoints*(sky_gridpoints+2)*2*6*sizeof(float);
	buf_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buf_desc.Usage = D3D11_USAGE_DEFAULT;

	subresource_data.pSysMem=sky_vertexdata;
	subresource_data.SysMemPitch=0;
	subresource_data.SysMemSlicePitch=0;

	result=pDevice->CreateBuffer(&buf_desc,&subresource_data,&sky_vertexbuffer);
	
	free (sky_vertexdata);
}

void CTerrain::LoadTextures()
{
	DirectX::CreateDDSTextureFromFile(pDevice, L"../media/TerrainTextures/rock_bump6.dds", (ID3D11Resource**)&rock_bump_texture, &rock_bump_textureSRV);
	DirectX::CreateDDSTextureFromFile(pDevice, L"../media/TerrainTextures/terrain_rock4.dds", (ID3D11Resource**)&rock_diffuse_texture, &rock_diffuse_textureSRV);
	DirectX::CreateDDSTextureFromFile(pDevice, L"../media/TerrainTextures/sand_diffuse.dds", (ID3D11Resource**)&sand_diffuse_texture, &sand_diffuse_textureSRV);
	DirectX::CreateDDSTextureFromFile(pDevice, L"../media/TerrainTextures/rock_bump4.dds", (ID3D11Resource**)&sand_bump_texture, &sand_bump_textureSRV);
	DirectX::CreateDDSTextureFromFile(pDevice, L"../media/TerrainTextures/terrain_grass.dds", (ID3D11Resource**)&grass_diffuse_texture, &grass_diffuse_textureSRV);
	DirectX::CreateDDSTextureFromFile(pDevice, L"../media/TerrainTextures/terrain_slope.dds", (ID3D11Resource**)&slope_diffuse_texture, &slope_diffuse_textureSRV);
	DirectX::CreateDDSTextureFromFile(pDevice, L"../media/TerrainTextures/lichen1_normal.dds", (ID3D11Resource**)&sand_microbump_texture, &sand_microbump_textureSRV);
	DirectX::CreateDDSTextureFromFile(pDevice, L"../media/TerrainTextures/rock_bump4.dds", (ID3D11Resource**)&rock_microbump_texture, &rock_microbump_textureSRV);
	DirectX::CreateDDSTextureFromFile(pDevice, L"../media/TerrainTextures/water_bump.dds", (ID3D11Resource**)&water_bump_texture, &water_bump_textureSRV);
	DirectX::CreateDDSTextureFromFile(pDevice, L"../media/TerrainTextures/sky.dds", (ID3D11Resource**)&sky_texture, &sky_textureSRV);
}


void CTerrain::Render(graphic::cRenderer &renderer, graphic::cCamera *cam)
{
	ID3D11DeviceContext* pContext;
	ID3DX11EffectShaderResourceVariable* tex_variable;
	float origin[2]={0,0};
	UINT stride=sizeof(float)*4;
	UINT offset=0;
	UINT cRT = 1;

	pDevice->GetImmediateContext(&pContext);

	tex_variable=pEffect->GetVariableByName("g_HeightfieldTexture")->AsShaderResource();
	tex_variable->SetResource(heightmap_textureSRV);

	tex_variable=pEffect->GetVariableByName("g_LayerdefTexture")->AsShaderResource();
	tex_variable->SetResource(layerdef_textureSRV);

	tex_variable=pEffect->GetVariableByName("g_RockBumpTexture")->AsShaderResource();
	tex_variable->SetResource(rock_bump_textureSRV);

	tex_variable=pEffect->GetVariableByName("g_RockMicroBumpTexture")->AsShaderResource();
	tex_variable->SetResource(rock_microbump_textureSRV);

	tex_variable=pEffect->GetVariableByName("g_RockDiffuseTexture")->AsShaderResource();
	tex_variable->SetResource(rock_diffuse_textureSRV);

	tex_variable=pEffect->GetVariableByName("g_SandBumpTexture")->AsShaderResource();
	tex_variable->SetResource(sand_bump_textureSRV);

	tex_variable=pEffect->GetVariableByName("g_SandMicroBumpTexture")->AsShaderResource();
	tex_variable->SetResource(sand_microbump_textureSRV);

	tex_variable=pEffect->GetVariableByName("g_SandDiffuseTexture")->AsShaderResource();
	tex_variable->SetResource(sand_diffuse_textureSRV);

	tex_variable=pEffect->GetVariableByName("g_GrassDiffuseTexture")->AsShaderResource();
	tex_variable->SetResource(grass_diffuse_textureSRV);

	tex_variable=pEffect->GetVariableByName("g_SlopeDiffuseTexture")->AsShaderResource();
	tex_variable->SetResource(slope_diffuse_textureSRV);

	tex_variable=pEffect->GetVariableByName("g_WaterBumpTexture")->AsShaderResource();
	tex_variable->SetResource(water_bump_textureSRV);

	tex_variable=pEffect->GetVariableByName("g_SkyTexture")->AsShaderResource();
	tex_variable->SetResource(sky_textureSRV);

	tex_variable=pEffect->GetVariableByName("g_DepthMapTexture")->AsShaderResource();
	tex_variable->SetResource(depthmap_textureSRV);

	pEffect->GetVariableByName("g_HeightFieldOrigin")->AsVector()->SetFloatVector(origin);
	pEffect->GetVariableByName("g_HeightFieldSize")->AsScalar()->SetFloat(terrain_gridpoints*terrain_geometry_scale);
	

 	ID3D11RenderTargetView *colorBuffer = renderer.m_renderTargetView;
	ID3D11DepthStencilView  *backBuffer = renderer.m_depthStencilView;
	D3D11_VIEWPORT currentViewport;
	D3D11_VIEWPORT reflection_Viewport;
	D3D11_VIEWPORT refraction_Viewport;
	D3D11_VIEWPORT shadowmap_resource_viewport;
	D3D11_VIEWPORT water_normalmap_resource_viewport;
	D3D11_VIEWPORT main_Viewport;

    float ClearColor[4] = { 0.8f, 0.8f, 1.0f, 1.0f };
	float RefractionClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

	reflection_Viewport.Width=(float)BackbufferWidth*reflection_buffer_size_multiplier;
	reflection_Viewport.Height=(float)BackbufferHeight*reflection_buffer_size_multiplier;
	reflection_Viewport.MaxDepth=1;
	reflection_Viewport.MinDepth=0;
	reflection_Viewport.TopLeftX=0;
	reflection_Viewport.TopLeftY=0;

	refraction_Viewport.Width=(float)BackbufferWidth*refraction_buffer_size_multiplier;
	refraction_Viewport.Height=(float)BackbufferHeight*refraction_buffer_size_multiplier;
	refraction_Viewport.MaxDepth=1;
	refraction_Viewport.MinDepth=0;
	refraction_Viewport.TopLeftX=0;
	refraction_Viewport.TopLeftY=0;

	main_Viewport.Width=(float)BackbufferWidth*main_buffer_size_multiplier;
	main_Viewport.Height=(float)BackbufferHeight*main_buffer_size_multiplier;
	main_Viewport.MaxDepth=1;
	main_Viewport.MinDepth=0;
	main_Viewport.TopLeftX=0;
	main_Viewport.TopLeftY=0;

	shadowmap_resource_viewport.Width=shadowmap_resource_buffer_size_xy;
	shadowmap_resource_viewport.Height=shadowmap_resource_buffer_size_xy;
	shadowmap_resource_viewport.MaxDepth=1;
	shadowmap_resource_viewport.MinDepth=0;
	shadowmap_resource_viewport.TopLeftX=0;
	shadowmap_resource_viewport.TopLeftY=0;

	water_normalmap_resource_viewport.Width=water_normalmap_resource_buffer_size_xy;
	water_normalmap_resource_viewport.Height=water_normalmap_resource_buffer_size_xy;
	water_normalmap_resource_viewport.MaxDepth=1;
	water_normalmap_resource_viewport.MinDepth=0;
	water_normalmap_resource_viewport.TopLeftX=0;
	water_normalmap_resource_viewport.TopLeftY=0;

	//1
	//saving scene color buffer and back buffer to constants
	pContext->RSGetViewports( &cRT, &currentViewport);
    pContext->OMGetRenderTargets( 1, &colorBuffer, &backBuffer );


	// selecting shadowmap_resource rendertarget
	pContext->RSSetViewports(1,&shadowmap_resource_viewport);
    pContext->OMSetRenderTargets( 0, NULL, shadowmap_resourceDSV);
    pContext->ClearDepthStencilView( shadowmap_resourceDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );

	//drawing terrain to depth buffer
	//SetupLightView(cam);
	//
	//pEffect->GetVariableByName("g_TerrainBeingRendered")->AsScalar()->SetFloat(1.0f);
	//pEffect->GetVariableByName("g_SkipCausticsCalculation")->AsScalar()->SetFloat(1.0f);

	//pContext->IASetInputLayout(heightfield_inputlayout);
	//pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
	//pEffect->GetTechniqueByName("RenderHeightfield")->GetPassByIndex(2)->Apply(0, pContext);
	//stride=sizeof(float)*4;
	//pContext->IASetVertexBuffers(0,1,&heightfield_vertexbuffer,&stride,&offset);
	//pContext->Draw(terrain_numpatches_1d*terrain_numpatches_1d, 0);
	


	pEffect->GetTechniqueByName("Default")->GetPassByIndex(0)->Apply(0, pContext);

	//if(g_RenderCaustics)
	if (0)
	{
		// selecting water_normalmap_resource rendertarget
		pContext->RSSetViewports(1,&water_normalmap_resource_viewport);
		pContext->OMSetRenderTargets( 1, &water_normalmap_resourceRTV, NULL);
		pContext->ClearRenderTargetView( water_normalmap_resourceRTV, ClearColor );

		//rendering water normalmap
		SetupNormalView(cam); // need it just to provide shader with camera position
		pContext->IASetInputLayout(trianglestrip_inputlayout);
		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		pEffect->GetTechniqueByName("WaterNormalmapCombine")->GetPassByIndex(0)->Apply(0, pContext);
		stride=sizeof(float)*6;
		pContext->IASetVertexBuffers(0,1,&heightfield_vertexbuffer,&stride,&offset);
		pContext->Draw(4, 0); // just need to pass 4 vertices to shader
		pEffect->GetTechniqueByName("Default")->GetPassByIndex(0)->Apply(0, pContext);
	}


	//2
	// setting up reflection rendertarget	
	pContext->RSSetViewports(1,&reflection_Viewport);
    pContext->OMSetRenderTargets( 1, &reflection_color_resourceRTV, reflection_depth_resourceDSV);
    pContext->ClearRenderTargetView( reflection_color_resourceRTV, RefractionClearColor );
    pContext->ClearDepthStencilView( reflection_depth_resourceDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );
	
	SetupReflectionView(cam);
	// drawing sky to reflection RT

	pEffect->GetTechniqueByName("RenderSky")->GetPassByIndex(g_RenderWireframe)->Apply(0, pContext);
	pContext->IASetInputLayout(trianglestrip_inputlayout);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	stride=sizeof(float)*6;
	pContext->IASetVertexBuffers(0,1,&sky_vertexbuffer,&stride,&offset);
	pContext->Draw(sky_gridpoints*(sky_gridpoints+2)*2, 0);

	//3
	// drawing terrain to reflection RT
	//tex_variable=pEffect->GetVariableByName("g_DepthTexture")->AsShaderResource();
	//tex_variable->SetResource(shadowmap_resourceSRV);

	//pEffect->GetVariableByName("g_SkipCausticsCalculation")->AsScalar()->SetFloat(1.0f);
	//pContext->IASetInputLayout(heightfield_inputlayout);
	//pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
	//pEffect->GetTechniqueByName("RenderHeightfield")->GetPassByIndex(g_RenderWireframe)->Apply(0, pContext);
	//stride=sizeof(float)*4;
	//pContext->IASetVertexBuffers(0,1,&heightfield_vertexbuffer,&stride,&offset);
	//pContext->Draw(terrain_numpatches_1d*terrain_numpatches_1d, 0);

	tex_variable=pEffect->GetVariableByName("g_DepthTexture")->AsShaderResource();
	tex_variable->SetResource(NULL);

	//pEffect->GetTechniqueByName("RenderHeightfield")->GetPassByIndex(g_RenderWireframe)->Apply(0, pContext);

	// setting up main rendertarget	
	pContext->RSSetViewports(1,&main_Viewport);
    pContext->OMSetRenderTargets( 1, &main_color_resourceRTV, main_depth_resourceDSV);
    pContext->ClearRenderTargetView( main_color_resourceRTV, ClearColor );
    pContext->ClearDepthStencilView( main_depth_resourceDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );
	SetupNormalView(cam);

	tex_variable=pEffect->GetVariableByName("g_WaterNormalMapTexture")->AsShaderResource();
	tex_variable->SetResource(water_normalmap_resourceSRV);

	// 4
	// drawing terrain to main buffer
	//tex_variable=pEffect->GetVariableByName("g_DepthTexture")->AsShaderResource();
	//tex_variable->SetResource(shadowmap_resourceSRV);
	//pEffect->GetVariableByName("g_TerrainBeingRendered")->AsScalar()->SetFloat(1.0f);
	//pEffect->GetVariableByName("g_SkipCausticsCalculation")->AsScalar()->SetFloat(0.0f);
	//pContext->IASetInputLayout(heightfield_inputlayout);
	//pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
	//pEffect->GetTechniqueByName("RenderHeightfield")->GetPassByIndex(g_RenderWireframe)->Apply(0, pContext);
	//stride=sizeof(float)*4;
	//pContext->IASetVertexBuffers(0,1,&heightfield_vertexbuffer,&stride,&offset);
	//pContext->Draw(terrain_numpatches_1d*terrain_numpatches_1d, 0);

	tex_variable=pEffect->GetVariableByName("g_WaterNormalMapTexture")->AsShaderResource();
	tex_variable->SetResource(NULL);
	tex_variable=pEffect->GetVariableByName("g_DepthTexture")->AsShaderResource();
	tex_variable->SetResource(NULL);
	//pEffect->GetTechniqueByName("RenderHeightfield")->GetPassByIndex(g_RenderWireframe)->Apply(0, pContext);

	// resolving main buffer color to refraction color resource
	pContext->ResolveSubresource(refraction_color_resource,0,main_color_resource,0,DXGI_FORMAT_R8G8B8A8_UNORM);

	// resolving main buffer depth to refraction depth resource manually
    pContext->RSSetViewports( 1, &main_Viewport );
	pContext->OMSetRenderTargets( 1, &refraction_depth_resourceRTV, NULL);
	float depthClear[4] = { 1,1,1,1 };
	pContext->ClearRenderTargetView(refraction_depth_resourceRTV, depthClear); // add test code

	// 5
	//pContext->IASetInputLayout(trianglestrip_inputlayout);
	//pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//switch(MultiSampleCount)
	//{
	//	case 1:
	//		tex_variable=pEffect->GetVariableByName("g_RefractionDepthTextureMS1")->AsShaderResource();
	//		tex_variable->SetResource(main_depth_resourceSRV);
	//		pEffect->GetTechniqueByName("RefractionDepthManualResolve")->GetPassByIndex(0)->Apply(0, pContext);
	//		break;
	//	case 2:
	//		tex_variable=pEffect->GetVariableByName("g_RefractionDepthTextureMS2")->AsShaderResource();
	//		tex_variable->SetResource(main_depth_resourceSRV);
	//		pEffect->GetTechniqueByName("RefractionDepthManualResolve")->GetPassByIndex(1)->Apply(0, pContext);
	//		break;
	//	case 4:
	//		tex_variable=pEffect->GetVariableByName("g_RefractionDepthTextureMS4")->AsShaderResource();
	//		tex_variable->SetResource(main_depth_resourceSRV);
	//		pEffect->GetTechniqueByName("RefractionDepthManualResolve")->GetPassByIndex(2)->Apply(0, pContext);
	//		break;
	//	default:
	//		tex_variable=pEffect->GetVariableByName("g_RefractionDepthTextureMS1")->AsShaderResource();
	//		tex_variable->SetResource(main_depth_resourceSRV);
	//		pEffect->GetTechniqueByName("RefractionDepthManualResolve1")->GetPassByIndex(0)->Apply(0, pContext);
	//		break;
	//}

	//stride=sizeof(float)*6;
	//pContext->IASetVertexBuffers(0,1,&heightfield_vertexbuffer,&stride,&offset);
	//pContext->Draw(4, 0); // just need to pass 4 vertices to shader

	//switch(MultiSampleCount)
	//{
	//	case 1:
	//		tex_variable=pEffect->GetVariableByName("g_RefractionDepthTextureMS1")->AsShaderResource();
	//		tex_variable->SetResource(NULL);
	//		pEffect->GetTechniqueByName("RefractionDepthManualResolve")->GetPassByIndex(0)->Apply(0, pContext);
	//		break;
	//	case 2:
	//		tex_variable=pEffect->GetVariableByName("g_RefractionDepthTextureMS2")->AsShaderResource();
	//		tex_variable->SetResource(NULL);
	//		pEffect->GetTechniqueByName("RefractionDepthManualResolve")->GetPassByIndex(1)->Apply(0, pContext);
	//		break;
	//	case 4:
	//		tex_variable=pEffect->GetVariableByName("g_RefractionDepthTextureMS4")->AsShaderResource();
	//		tex_variable->SetResource(NULL);
	//		pEffect->GetTechniqueByName("RefractionDepthManualResolve")->GetPassByIndex(2)->Apply(0, pContext);
	//		break;
	//	default:
	//		tex_variable=pEffect->GetVariableByName("g_RefractionDepthTextureMS1")->AsShaderResource();
	//		tex_variable->SetResource(NULL);
	//		pEffect->GetTechniqueByName("RefractionDepthManualResolve1")->GetPassByIndex(0)->Apply(0, pContext);
	//		break;
	//}

	// getting back to rendering to main buffer 
	pContext->RSSetViewports(1,&main_Viewport);
    pContext->OMSetRenderTargets( 1, &main_color_resourceRTV, main_depth_resourceDSV);

	// drawing water surface to main buffer

	tex_variable=pEffect->GetVariableByName("g_DepthTexture")->AsShaderResource();
	//tex_variable->SetResource(shadowmap_resourceSRV);
	tex_variable->SetResource(NULL);
	tex_variable=pEffect->GetVariableByName("g_ReflectionTexture")->AsShaderResource();
	tex_variable->SetResource(reflection_color_resourceSRV);
	//tex_variable->SetResource(NULL);
	tex_variable=pEffect->GetVariableByName("g_RefractionTexture")->AsShaderResource();
	//tex_variable->SetResource(refraction_color_resourceSRV);
	tex_variable->SetResource(NULL);
	tex_variable=pEffect->GetVariableByName("g_RefractionDepthTextureResolved")->AsShaderResource();
	tex_variable->SetResource(refraction_depth_resourceSRV);
	//tex_variable->SetResource(NULL);
	tex_variable=pEffect->GetVariableByName("g_WaterNormalMapTexture")->AsShaderResource();
	//tex_variable->SetResource(water_normalmap_resourceSRV);
	tex_variable->SetResource(NULL); // add test code

	pEffect->GetVariableByName("g_TerrainBeingRendered")->AsScalar()->SetFloat(0.0f);
	pEffect->GetTechniqueByName("RenderWater")->GetPassByIndex(g_RenderWireframe)->Apply(0, pContext);

	pContext->IASetInputLayout(heightfield_inputlayout);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
	stride=sizeof(float)*4;
	pContext->IASetVertexBuffers(0,1,&heightfield_vertexbuffer,&stride,&offset);
	pContext->Draw(terrain_numpatches_1d*terrain_numpatches_1d, 0);



	tex_variable=pEffect->GetVariableByName("g_DepthTexture")->AsShaderResource();
	tex_variable->SetResource(NULL);
	tex_variable=pEffect->GetVariableByName("g_ReflectionTexture")->AsShaderResource();
	tex_variable->SetResource(NULL);
	tex_variable=pEffect->GetVariableByName("g_RefractionTexture")->AsShaderResource();
	tex_variable->SetResource(NULL);
	tex_variable=pEffect->GetVariableByName("g_RefractionDepthTextureResolved")->AsShaderResource();
	tex_variable->SetResource(NULL);
	tex_variable=pEffect->GetVariableByName("g_WaterNormalMapTexture")->AsShaderResource();
	tex_variable->SetResource(NULL);
	pEffect->GetTechniqueByName("RenderWater")->GetPassByIndex(g_RenderWireframe)->Apply(0, pContext);

	// 6
	//drawing sky to main buffer
	pEffect->GetTechniqueByName("RenderSky")->GetPassByIndex(g_RenderWireframe)->Apply(0, pContext);

	pContext->IASetInputLayout(trianglestrip_inputlayout);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	stride=sizeof(float)*6;
	pContext->IASetVertexBuffers(0,1,&sky_vertexbuffer,&stride,&offset);
	pContext->Draw(sky_gridpoints*(sky_gridpoints+2)*2, 0);

	pEffect->GetTechniqueByName("Default")->GetPassByIndex(0)->Apply(0, pContext);

	//restoring scene color buffer and back buffer
	pContext->OMSetRenderTargets( 1, &colorBuffer, backBuffer);
    pContext->RSSetViewports( 1, &currentViewport );

	//resolving main buffer 
	pContext->ResolveSubresource(main_color_resource_resolved,0,main_color_resource,0,DXGI_FORMAT_R8G8B8A8_UNORM);

	//drawing main buffer to back buffer
	tex_variable=pEffect->GetVariableByName("g_MainTexture")->AsShaderResource();
	tex_variable->SetResource(main_color_resource_resolvedSRV);
	pEffect->GetVariableByName("g_MainBufferSizeMultiplier")->AsScalar()->SetFloat(main_buffer_size_multiplier);

	pContext->IASetInputLayout(trianglestrip_inputlayout);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pEffect->GetTechniqueByName("MainToBackBuffer")->GetPassByIndex(0)->Apply(0, pContext);
	stride=sizeof(float)*6;
	pContext->IASetVertexBuffers(0,1,&heightfield_vertexbuffer,&stride,&offset);
	pContext->Draw(4, 0); // just need to pass 4 vertices to shader

	tex_variable=pEffect->GetVariableByName("g_MainTexture")->AsShaderResource();
	tex_variable->SetResource(NULL);

	pEffect->GetTechniqueByName("Default")->GetPassByIndex(0)->Apply(0, pContext);

    SAFE_RELEASE ( colorBuffer );
    SAFE_RELEASE ( backBuffer );

	pContext->Release();
}

float bilinear_interpolation(float fx, float fy, float a, float b, float c, float d)
{
	float s1,s2,s3,s4;
	s1=fx*fy;
	s2=(1-fx)*fy;
	s3=(1-fx)*(1-fy);
	s4=fx*(1-fy);
	return((a*s3+b*s4+c*s1+d*s2));
}

void CTerrain::SetupReflectionView(graphic::cCamera *cam)
{

	float aspectRatio = BackbufferWidth / BackbufferHeight;

	Vector3 EyePoint;
	Vector3 LookAtPoint;

	EyePoint = cam->GetEyePos();
	LookAtPoint = cam->GetLookAt();
	EyePoint.y=-1.0f*EyePoint.y+1.0f;
	LookAtPoint.y=-1.0f*LookAtPoint.y+1.0f;


	Matrix44 mView;
	Matrix44 mProj;
	Matrix44 mViewProj;
	Matrix44 mViewProjInv;

	Matrix44 mWorld;
	mView = cam->GetViewMatrix();
	//mWorld = cam->GetWorldMatrix();
	mWorld = cam->GetViewMatrix().Inverse();

	mWorld._42=-mWorld._42-1.0f;
	
	mWorld._21*=-1.0f;
	mWorld._23*=-1.0f;

	mWorld._32*=-1.0f;
	
	//D3DXMatrixInverse(&mView, NULL, &mWorld);
	mView = mWorld.Inverse();
	//D3DXMatrixPerspectiveFovLH(&mProj,camera_fov * D3DX_PI / 360.0f,aspectRatio,scene_z_near,scene_z_far);
	mProj.SetProjection(camera_fov * MATH_PI / 360.0f, aspectRatio, scene_z_near, scene_z_far);
	mViewProj=mView*mProj;
	//D3DXMatrixInverse(&mViewProjInv, NULL, &mViewProj);
	mViewProjInv = mViewProj.Inverse();

	pEffect->GetVariableByName("g_ModelViewProjectionMatrix")->AsMatrix()->SetMatrix((float*)&mViewProj);
	pEffect->GetVariableByName("g_CameraPosition")->AsVector()->SetFloatVector((float*)&EyePoint);
	Vector3 direction = LookAtPoint - EyePoint;
	//Vector3 normalized_direction = *D3DXVec3Normalize(&normalized_direction,&direction);
	Vector3 normalized_direction = direction.Normal();
	pEffect->GetVariableByName("g_CameraDirection")->AsVector()->SetFloatVector((float*)&normalized_direction);

	pEffect->GetVariableByName("g_HalfSpaceCullSign")->AsScalar()->SetFloat(1.0f);
	pEffect->GetVariableByName("g_HalfSpaceCullPosition")->AsScalar()->SetFloat(-0.6f);
}

void CTerrain::SetupRefractionView(graphic::cCamera *cam)
{
	pEffect->GetVariableByName("g_HalfSpaceCullSign")->AsScalar()->SetFloat(-1.0f);
	pEffect->GetVariableByName("g_HalfSpaceCullPosition")->AsScalar()->SetFloat(terrain_minheight);
}
void CTerrain::SetupLightView(graphic::cCamera *cam)
{
	Vector3 EyePoint(-10000.0f,6500.0f,10000.0f);
	Vector3 LookAtPoint(terrain_far_range/2.0f,0.0f,terrain_far_range/2.0f);
	Vector3 lookUp(0,1,0);
	Vector3 cameraPosition = cam->GetEyePos();

	float nr, fr;
	nr=sqrt(EyePoint.x*EyePoint.x+EyePoint.y*EyePoint.y+EyePoint.z*EyePoint.z)-terrain_far_range*0.7f;
	fr=sqrt(EyePoint.x*EyePoint.x+EyePoint.y*EyePoint.y+EyePoint.z*EyePoint.z)+terrain_far_range*0.7f;

	Matrix44 mView;
	mView.SetView2(EyePoint, LookAtPoint, lookUp);
	Matrix44 mProjMatrix;
	mProjMatrix.SetProjectionOrthogonal(terrain_far_range*1.5, terrain_far_range, nr, fr);
	Matrix44 mViewProj = mView * mProjMatrix;
	Matrix44 mViewProjInv;
	mViewProjInv = mViewProj.Inverse();

	pEffect->GetVariableByName("g_ModelViewProjectionMatrix")->AsMatrix()->SetMatrix((float*)&mViewProj);
	pEffect->GetVariableByName("g_LightModelViewProjectionMatrix")->AsMatrix()->SetMatrix((float*)&mViewProj);
	pEffect->GetVariableByName("g_LightModelViewProjectionMatrixInv")->AsMatrix()->SetMatrix((float*)&mViewProjInv);
	pEffect->GetVariableByName("g_CameraPosition")->AsVector()->SetFloatVector((float*)&cameraPosition);
	Vector3 direction = cam->GetLookAt() - cam->GetEyePos();
	Vector3 normalized_direction = direction.Normal();
	pEffect->GetVariableByName("g_CameraDirection")->AsVector()->SetFloatVector((float*)&normalized_direction);

	pEffect->GetVariableByName("g_HalfSpaceCullSign")->AsScalar()->SetFloat(1.0);
	pEffect->GetVariableByName("g_HalfSpaceCullPosition")->AsScalar()->SetFloat(terrain_minheight*2);	
}

void CTerrain::SetupNormalView(graphic::cCamera *cam)
{
	Vector3 EyePoint;
	Vector3 LookAtPoint;

	EyePoint = cam->GetEyePos();
	LookAtPoint = cam->GetLookAt();
	Matrix44 mView = cam->GetViewMatrix();
	Matrix44 mProjMatrix = cam->GetProjectionMatrix();
	Matrix44 mViewProj = mView * mProjMatrix;
	Matrix44 mViewProjInv;
    //D3DXMatrixInverse(&mViewProjInv, NULL, &mViewProj);
	mViewProjInv = mViewProj.Inverse();
	Vector3 cameraPosition = cam->GetEyePos();

	pEffect->GetVariableByName("g_ModelViewMatrix")->AsMatrix()->SetMatrix((float*)&mView);
	pEffect->GetVariableByName("g_ModelViewProjectionMatrix")->AsMatrix()->SetMatrix((float*)&mViewProj);
	pEffect->GetVariableByName("g_ModelViewProjectionMatrixInv")->AsMatrix()->SetMatrix((float*)&mViewProjInv);
	pEffect->GetVariableByName("g_CameraPosition")->AsVector()->SetFloatVector((float*)&cameraPosition);
	Vector3 direction = LookAtPoint - EyePoint;
	//Vector3 normalized_direction = *D3DXVec3Normalize(&normalized_direction,&direction);
	Vector3 normalized_direction = direction.Normal();
	pEffect->GetVariableByName("g_CameraDirection")->AsVector()->SetFloatVector((float*)&normalized_direction);
	pEffect->GetVariableByName("g_HalfSpaceCullSign")->AsScalar()->SetFloat(1.0);
	pEffect->GetVariableByName("g_HalfSpaceCullPosition")->AsScalar()->SetFloat(terrain_minheight*2);
}
