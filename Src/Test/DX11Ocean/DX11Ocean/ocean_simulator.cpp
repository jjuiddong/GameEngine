//--------------------------------------------------------------------------------------
// File: ocean_simulator.cpp
//
// Main class of ocean simulation
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

// Disable warning "conditional expression is constant"
#pragma warning(disable:4127)

//#include "utilities.h"
//#include "DXUT.h"
//#include "SDKmisc.h"
#include "stdafx.h"
#include "ocean_simulator.h"
#include <amp_math.h>

#define HALF_SQRT_2	0.7071068f
#define GRAV_ACCEL	981.0f	// The acceleration of gravity, cm/s^2

extern __int64 g_freq;

// Generating gaussian random number with mean 0 and standard deviation 1.
float gauss()
{
    float u1 = rand() / (float)RAND_MAX;
    float u2 = rand() / (float)RAND_MAX;
    if (u1 < 1e-6f)
        u1 = 1e-6f;
    return sqrtf(-2 * logf(u1)) * cosf(2*MATH_PI * u2);
}

// phillips Spectrum
// K: normalized wave vector, W: wind direction, v: wind velocity, a: amplitude constant
float phillips(Vector2 K, Vector2 W, float v, float a, float dir_depend)
{
    // largest possible wave from constant wind of velocity v
    float l = v * v / GRAV_ACCEL;
    // damp out waves with very small length w << l
    float w = l / 1000;

    float Ksqr = K.x * K.x + K.y * K.y;
    float Kcos = K.x * W.x + K.y * W.y;
    float phillips = a * expf(-1 / (l * l * Ksqr)) / (Ksqr * Ksqr * Ksqr) * (Kcos * Kcos);

    // filter out waves moving opposite to wind
    if (Kcos < 0)
        phillips *= dir_depend;

    // damp out waves with very small length w << l
    return phillips * expf(-Ksqr * w * w);
}


// Pre-FFT data preparation
void update_spectrum(int dimY,
                     int dimX,
                     array_view<const float_2> input_h0,
                     array_view<const float> input_omega,
                     array_view<float_2> output_ht, 
                     immutable immutable,
                     change_per_frame perframe,
                     accelerator_view av)
{
    extent<2> update_spectrum(dimY, dimX);

    parallel_for_each(av, update_spectrum, [=] (index<2> idx) restrict(amp)
    {
        index<1> in_index = index<1>(idx[0] * immutable.inwidth + idx[1]);
        index<1> in_mindex = index<1>((immutable.actualdim - idx[0]) * immutable.inwidth + (immutable.actualdim - idx[1]));
        index<1> out_index = index<1>(idx[0] * immutable.outwidth + idx[1]);

        // H(0) -> H(t)
        float_2 h0_k  = input_h0[in_index];
        float_2 h0_mk = input_h0[in_mindex];
        float sin_v, cos_v;
        fast_math::sincos(input_omega[in_index] * perframe.time, &sin_v, &cos_v);

        float_2 ht;
        ht.x = (h0_k.x + h0_mk.x) * cos_v - (h0_k.y + h0_mk.y) * sin_v;
        ht.y = (h0_k.x - h0_mk.x) * sin_v + (h0_k.y - h0_mk.y) * cos_v;

        // H(t) -> Dx(t), Dy(t)
        float kx = idx[1] - immutable.actualdim * 0.5f;
        float ky = idx[0] - immutable.actualdim * 0.5f;
        float sqr_k = kx * kx + ky * ky;
        float rsqr_k = 0;
        if (sqr_k > 1e-12f) {
            rsqr_k = 1 / fast_math::sqrt(sqr_k);
        }
        kx *= rsqr_k;
        ky *= rsqr_k;

        float_2 dt_x;

        dt_x.x = ht.y * kx;
        dt_x.y = -ht.x * kx;

        float_2 dt_y;

        dt_y.x = ht.y * ky;
        dt_y.y = -ht.x * ky;

        if ((idx[1] < immutable.outwidth) && (idx[0] < immutable.outwidth))
        {
            output_ht[out_index] = ht;
            output_ht[out_index + immutable.dddressoffset] = dt_x;
            output_ht[out_index + immutable.addressoffset] = dt_y;		
        }    
    });    
}

ocean_simulator::ocean_simulator(ocean_parameter& params, ID3D11Device* pd3dDevice)
	: m_pd3dDevice(pd3dDevice),
	m_av(concurrency::direct3d::create_accelerator_view(reinterpret_cast<IUnknown *>(pd3dDevice))),
	m_array_view_float2_h0(NULL),
	m_array_view_float2_ht(NULL), 
	m_array_view_float_omega(NULL),
	m_array_view_float_dxyz(NULL),
	m_fft_plan(3, m_av)
{
    // If the device becomes invalid at some point, delete current instance and generate a new one.
    assert(m_pd3dDevice);
    m_pd3dDevice->GetImmediateContext(&m_pd3dImmediateContext);
    assert(m_pd3dImmediateContext);

    // Height map H(0)
    int height_map_size = (params.dmap_dim + 4) * (params.dmap_dim + 1);
    vector<float_2> h0_data(height_map_size);
    memset(h0_data.data(), 0, height_map_size * sizeof(Vector2));
    vector<float> omega_data(height_map_size);
    memset(omega_data.data(), 0, height_map_size * sizeof(float));
    init_height_map(params, h0_data, omega_data);

    m_param = params;
    int hmap_dim = params.dmap_dim;
    // This value should be (hmap_dim / 2 + 1) * hmap_dim, but right now we can only use full size.
    int input_half_size = hmap_dim * hmap_dim;
    int output_size = hmap_dim * hmap_dim;

    // For filling the buffer with zeroes.
    vector<float_2> zero_data(3 * output_size);
    memset(zero_data.data(), 0, 3 * output_size * sizeof(float_2));

    UINT float2_stride = 2 * sizeof(float);

    init_buffers(params, h0_data, omega_data, zero_data);

    init_dx11(hmap_dim, output_size, float2_stride);
}

ocean_simulator::~ocean_simulator()
{
    m_pd3dImmediateContext->ClearState();
    delete m_array_view_float2_h0;
    m_array_view_float2_h0 = NULL;

    delete m_array_view_float_omega;
    m_array_view_float_omega = NULL;

    delete m_array_view_float2_ht;
    m_array_view_float2_ht = NULL;

    delete m_array_view_float_dxyz;
    m_array_view_float_dxyz = NULL;

    SAFE_RELEASE(m_pBuffer_Float_Dxyz);

    SAFE_RELEASE(m_pPointSamplerState);

    SAFE_RELEASE(m_pQuadVB);

    SAFE_RELEASE(m_pUAV_Dxyz);
    SAFE_RELEASE(m_pSRV_Dxyz);

    SAFE_RELEASE(m_pDisplacementMap);
    SAFE_RELEASE(m_pDisplacementSRV);
    SAFE_RELEASE(m_pDisplacementRTV);

    SAFE_RELEASE(m_pGradientMap);
    SAFE_RELEASE(m_pGradientSRV);
    SAFE_RELEASE(m_pGradientRTV);

    SAFE_RELEASE(m_pQuadVS);
    SAFE_RELEASE(m_pUpdateDisplacementPS);
    SAFE_RELEASE(m_pGenGradientFoldingPS);

    SAFE_RELEASE(m_pQuadLayout);

    SAFE_RELEASE(m_pImmutableCB);
    SAFE_RELEASE(m_pPerFrameCB);

    SAFE_RELEASE(m_pd3dImmediateContext);
}


// Initialize the vector Array.
// wlen_x: width of wave tile, in meters
// wlen_y: length of wave tile, in meters
void ocean_simulator::init_height_map(ocean_parameter& params, vector<float_2> &out_h0, vector<float> &out_omega)
{
    int i, j;
	Vector2 K, Kn;

	Vector2 wind_dir;
    //D3DXVec2Normalize(&wind_dir, &params.wind_dir);
	wind_dir = params.wind_dir.Normal();
    float a = params.wave_amplitude * 1e-7f;	// It is too small. We must scale it for editing.
    float v = params.wind_speed;
    float dir_depend = params.wind_dependency;

    int height_map_dim = params.dmap_dim;
    float patch_length = params.patch_length;

    // initialize random generator.
    srand(0);

    for (i = 0; i <= height_map_dim; i++)
    {
        // K is wave-vector, range [-|DX/W, |DX/W], [-|DY/H, |DY/H]
        K.y = (-height_map_dim / 2.0f + i) * (2 * MATH_PI / patch_length);

        for (j = 0; j <= height_map_dim; j++)
        {
            K.x = (-height_map_dim / 2.0f + j) * (2 * MATH_PI / patch_length);

            float phil = (K.x == 0 && K.y == 0) ? 0 : sqrtf(phillips(K, wind_dir, v, a, dir_depend));

            out_h0[i * (height_map_dim + 4) + j].x = float(phil * gauss() * 0.7071068f);
            out_h0[i * (height_map_dim + 4) + j].y = float(phil * gauss() * 0.7071068f);

            // The angular frequency is following the dispersion relation:
            //            out_omega^2 = g*k
            // So the equation of Gerstner wave is:
            //            x = x0 - K/k * A * sin(dot(K, x0) - sqrt(g * k) * t), x is a 2D vector.
            //            z = A * cos(dot(K, x0) - sqrt(g * k) * t)
            // Gerstner wave shows that a point on a simple sinusoid wave is doing a uniform circular
            // motion with the center at (x0, y0, z0), radius at A, and the circular plane is parallel
            // to vector K.
            out_omega[i * (height_map_dim + 4) + j] = sqrtf(GRAV_ACCEL * sqrtf(K.x * K.x + K.y * K.y));
        }
    }
}


// Key function: first, use amp to calcuate the frequency by using time value. Secondly, use amp to calculate the space information by using frequency.
void ocean_simulator::update_displacement_map(float time)
{
    // ---------------------------- H(0) -> H(t), D(x, t), D(y, t) --------------------------------
    // Consts
    D3D11_MAPPED_SUBRESOURCE mapped_res;            
    m_pd3dImmediateContext->Map(m_pPerFrameCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res);
    assert(mapped_res.pData);
    float* per_frame_data = (float*)mapped_res.pData;
    // time
    per_frame_data[0] = time * m_param.time_scale;
    // choppyscale
    per_frame_data[1] = m_param.choppy_scale;
    // g_GridLen
    per_frame_data[2] = m_param.dmap_dim / m_param.patch_length;
    m_pd3dImmediateContext->Unmap(m_pPerFrameCB, 0);

    this->m_cbchange_per_frame.time = per_frame_data[0];
    this->m_cbchange_per_frame.choppyscale = per_frame_data[1];

    // Pre-FFT data preparation:
    update_spectrum(m_param.dmap_dim, m_param.dmap_dim, 
        *m_array_view_float2_h0, *m_array_view_float_omega, *m_array_view_float2_ht,
        this->m_cbimmutable, this->m_cbchange_per_frame, this->m_av);

    // ------------------------------------ Perform FFT -------------------------------------------
    m_fft_plan.fft_512x512_c2c_amp(*m_array_view_float_dxyz, *m_array_view_float2_ht);

    // --------------------------------- Wrap Dx, Dy and Dz ---------------------------------------
    // Push RT
    ID3D11RenderTargetView* old_target;
    ID3D11DepthStencilView* old_depth;
    m_pd3dImmediateContext->OMGetRenderTargets(1, &old_target, &old_depth); 
    D3D11_VIEWPORT old_viewport;
    UINT num_viewport = 1;
    m_pd3dImmediateContext->RSGetViewports(&num_viewport, &old_viewport);

    D3D11_VIEWPORT new_vp = {0, 0, (float)m_param.dmap_dim, (float)m_param.dmap_dim, 0.0f, 1.0f};
    m_pd3dImmediateContext->RSSetViewports(1, &new_vp);

    // Set RT
    ID3D11RenderTargetView* rt_views[1] = {m_pDisplacementRTV};
    m_pd3dImmediateContext->OMSetRenderTargets(1, rt_views, NULL);

    // VS & PS
    m_pd3dImmediateContext->VSSetShader(m_pQuadVS, NULL, 0);
    m_pd3dImmediateContext->PSSetShader(m_pUpdateDisplacementPS, NULL, 0);

    // Constants
    ID3D11Buffer* ps_cbs[2] = {m_pImmutableCB, m_pPerFrameCB};
    m_pd3dImmediateContext->PSSetConstantBuffers(0, 2, ps_cbs);

    // Buffer resources
    ID3D11ShaderResourceView* ps_srvs[1] = {m_pSRV_Dxyz};
    m_pd3dImmediateContext->PSSetShaderResources(0, 1, ps_srvs);

    // IA setup
    ID3D11Buffer* vbs[1] = {m_pQuadVB};
    UINT strides[1] = {sizeof(Vector4)};
    UINT offsets[1] = {0};
    m_pd3dImmediateContext->IASetVertexBuffers(0, 1, &vbs[0], &strides[0], &offsets[0]);

    m_pd3dImmediateContext->IASetInputLayout(m_pQuadLayout);
    m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // Perform draw call
    m_pd3dImmediateContext->Draw(4, 0);

    // Unbind
    ps_srvs[0] = NULL;
    m_pd3dImmediateContext->PSSetShaderResources(0, 1, ps_srvs);


    // ----------------------------------- Generate Normal ----------------------------------------
    // Set RT
    rt_views[0] = m_pGradientRTV;
    m_pd3dImmediateContext->OMSetRenderTargets(1, rt_views, NULL);

    // VS & PS
    m_pd3dImmediateContext->VSSetShader(m_pQuadVS, NULL, 0);
    m_pd3dImmediateContext->PSSetShader(m_pGenGradientFoldingPS, NULL, 0);

    // Texture resource and sampler
    ps_srvs[0] = m_pDisplacementSRV;
    m_pd3dImmediateContext->PSSetShaderResources(0, 1, ps_srvs);

    ID3D11SamplerState* samplers[1] = {m_pPointSamplerState};
    m_pd3dImmediateContext->PSSetSamplers(0, 1, &samplers[0]);

    // Perform draw call
    m_pd3dImmediateContext->Draw(4, 0);

    // Unbind
    ps_srvs[0] = NULL;
    m_pd3dImmediateContext->PSSetShaderResources(0, 1, ps_srvs);

    // Pop RT
    m_pd3dImmediateContext->RSSetViewports(1, &old_viewport);
    m_pd3dImmediateContext->OMSetRenderTargets(1, &old_target, old_depth);
    SAFE_RELEASE(old_target);
    SAFE_RELEASE(old_depth);

    m_pd3dImmediateContext->GenerateMips(m_pGradientSRV);
}


ID3D11ShaderResourceView* ocean_simulator::get_direct3d_displacement_map()
{
    return m_pDisplacementSRV;
}

ID3D11ShaderResourceView* ocean_simulator::get_direct3d_gradient_map()
{
    return m_pGradientSRV;
}


const ocean_parameter& ocean_simulator::get_parameters()
{
    return m_param;
}



void ocean_simulator::direct3d_synchronize()
{
    ID3D11Query *queryHandle = NULL;
    D3D11_QUERY_DESC queryDesc = { D3D11_QUERY_EVENT, 0 };

    m_pd3dDevice->CreateQuery(&queryDesc, &queryHandle);

    HRESULT hr;
    do {
        hr = m_pd3dImmediateContext->GetData(reinterpret_cast<ID3D11Asynchronous*>(queryHandle), NULL, 0, 0);
    } while (hr == S_FALSE);

    queryHandle->Release();
}

void ocean_simulator::init_buffers(ocean_parameter& params, vector<float_2> &h0_data, vector<float> &omega_data, vector<float_2> &zero_data)
{
    // H0
    m_array_view_float2_h0 = new array_view<const float_2>(array<float_2>((params.dmap_dim + 4) * (params.dmap_dim + 1), h0_data.begin(), m_av));

    // Put H(t), Dx(t) and Dy(t) into one buffer
    m_array_view_float2_ht = new array_view<float_2>(array<float_2>(3 * params.dmap_dim * params.dmap_dim, zero_data.begin(), m_av));

    // omega
    m_array_view_float_omega = new array_view<const float>(array<float>((params.dmap_dim + 4) * (params.dmap_dim + 1), omega_data.begin(), m_av));

    array<float_2> tmp_array(3 * params.dmap_dim * params.dmap_dim, zero_data.begin(), m_av);
    m_array_view_float_dxyz = new array_view<float_2>(tmp_array);
    m_pBuffer_Float_Dxyz = reinterpret_cast<ID3D11Buffer *>(Concurrency::direct3d::get_buffer(tmp_array));
}


void ocean_simulator::init_dx11(int hmap_dim, int output_size, UINT float2_stride)
{
    graphic::createUAV(m_pd3dDevice, m_pBuffer_Float_Dxyz, 3 * output_size * float2_stride, sizeof(float), &m_pUAV_Dxyz, &m_pSRV_Dxyz);

    // D3D11 Textures
	graphic::createTextureAndViews(m_pd3dDevice, hmap_dim, hmap_dim, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_pDisplacementMap, &m_pDisplacementSRV, &m_pDisplacementRTV);
	graphic::createTextureAndViews(m_pd3dDevice, hmap_dim, hmap_dim, DXGI_FORMAT_R16G16B16A16_FLOAT, &m_pGradientMap, &m_pGradientSRV, &m_pGradientRTV);

    // Samplers
    D3D11_SAMPLER_DESC sam_desc;
    sam_desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    sam_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sam_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sam_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sam_desc.MipLODBias = 0; 
    sam_desc.MaxAnisotropy = 1; 
    sam_desc.ComparisonFunc = D3D11_COMPARISON_NEVER; 
    sam_desc.BorderColor[0] = 1.0f;
    sam_desc.BorderColor[1] = 1.0f;
    sam_desc.BorderColor[2] = 1.0f;
    sam_desc.BorderColor[3] = 1.0f;
    sam_desc.MinLOD = -FLT_MAX;
    sam_desc.MaxLOD = FLT_MAX;
    m_pd3dDevice->CreateSamplerState(&sam_desc, &m_pPointSamplerState);
    assert(m_pPointSamplerState);

    // Vertex & pixel shaders
    ID3DBlob* pBlobQuadVS = NULL;
    ID3DBlob* pBlobUpdateDisplacementPS = NULL;
    ID3DBlob* pBlobGenGradientFoldingPS = NULL;

    wstring filePath = L"../media/ocean/ocean_simulator_vs_ps.hlsl";

    graphic::CompileShaderFromFile((WCHAR *)filePath.c_str(), "QuadVS", "vs_4_0", &pBlobQuadVS);
	graphic::CompileShaderFromFile((WCHAR *)filePath.c_str(), "UpdateDisplacementPS", "ps_4_0", &pBlobUpdateDisplacementPS);
	graphic::CompileShaderFromFile((WCHAR *)filePath.c_str(), "GenGradientFoldingPS", "ps_4_0", &pBlobGenGradientFoldingPS);
    assert(pBlobQuadVS);
    assert(pBlobUpdateDisplacementPS);
    assert(pBlobGenGradientFoldingPS);

    m_pd3dDevice->CreateVertexShader(pBlobQuadVS->GetBufferPointer(), pBlobQuadVS->GetBufferSize(), NULL, &m_pQuadVS);
    m_pd3dDevice->CreatePixelShader(pBlobUpdateDisplacementPS->GetBufferPointer(), pBlobUpdateDisplacementPS->GetBufferSize(), NULL, &m_pUpdateDisplacementPS);
    m_pd3dDevice->CreatePixelShader(pBlobGenGradientFoldingPS->GetBufferPointer(), pBlobGenGradientFoldingPS->GetBufferSize(), NULL, &m_pGenGradientFoldingPS);
    assert(m_pQuadVS);
    assert(m_pUpdateDisplacementPS);
    assert(m_pGenGradientFoldingPS);
    SAFE_RELEASE(pBlobUpdateDisplacementPS);
    SAFE_RELEASE(pBlobGenGradientFoldingPS);

    // Input layout
    D3D11_INPUT_ELEMENT_DESC quad_layout_desc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    m_pd3dDevice->CreateInputLayout(quad_layout_desc, 1, pBlobQuadVS->GetBufferPointer(), pBlobQuadVS->GetBufferSize(), &m_pQuadLayout);
    assert(m_pQuadLayout);

    SAFE_RELEASE(pBlobQuadVS);

    // Quad vertex buffer
    D3D11_BUFFER_DESC vb_desc;
    vb_desc.ByteWidth = 4 * sizeof(Vector4);
    vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
    vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vb_desc.CPUAccessFlags = 0;
    vb_desc.MiscFlags = 0;

    float quad_verts[] =
    {
        -1, -1, 0, 1,
        -1,  1, 0, 1,
        1, -1, 0, 1,
        1,  1, 0, 1,
    };
    D3D11_SUBRESOURCE_DATA init_data;
    init_data.pSysMem = &quad_verts[0];
    init_data.SysMemPitch = 0;
    init_data.SysMemSlicePitch = 0;

    m_pd3dDevice->CreateBuffer(&vb_desc, &init_data, &m_pQuadVB);
    assert(m_pQuadVB);

    // Constant buffers
    UINT actual_dim = m_param.dmap_dim;
    UINT input_width = actual_dim + 4;
    // We use full sized data here. The value "output_width" should be actual_dim/2+1 though.
    UINT output_width = actual_dim;
    UINT output_height = actual_dim;
    UINT dtx_offset = actual_dim * actual_dim;
    UINT dty_offset = actual_dim * actual_dim * 2;
    UINT immutable_consts[] = {actual_dim, input_width, output_width, output_height, dtx_offset, dty_offset};
    memcpy(&this->m_cbimmutable, immutable_consts, sizeof(immutable_consts));

    D3D11_SUBRESOURCE_DATA init_cb0 = {&immutable_consts[0], 0, 0};

    D3D11_BUFFER_DESC cb_desc;
    cb_desc.Usage = D3D11_USAGE_IMMUTABLE;
    cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb_desc.CPUAccessFlags = 0;
    cb_desc.MiscFlags = 0;    
    cb_desc.ByteWidth = PAD16(sizeof(immutable_consts));
    m_pd3dDevice->CreateBuffer(&cb_desc, &init_cb0, &m_pImmutableCB);
    assert(m_pImmutableCB);

    ID3D11Buffer* cbs[1] = {m_pImmutableCB};
    m_pd3dImmediateContext->CSSetConstantBuffers(0, 1, cbs);
    m_pd3dImmediateContext->PSSetConstantBuffers(0, 1, cbs);

    cb_desc.Usage = D3D11_USAGE_DYNAMIC;
    cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cb_desc.MiscFlags = 0;    
    cb_desc.ByteWidth = PAD16(sizeof(float) * 3);
    m_pd3dDevice->CreateBuffer(&cb_desc, NULL, &m_pPerFrameCB);
    assert(m_pPerFrameCB);
}
