//--------------------------------------------------------------------------------------
// File: fft_512x512.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#ifndef _FFT_512_512_H
#define _FFT_512_512_H

#include <amp.h>
#include <amp_math.h>
#include <amp_short_vectors.h>
//#include "DXUT.h"

using namespace concurrency;
using namespace concurrency::graphics;

////////////////////////////////////////////////////////////////////////////////
// Common constants
////////////////////////////////////////////////////////////////////////////////
#define TWO_PI 6.283185307179586476925286766559
#define MAX_BUF_LEN 0x600000
#define COS_PI_4_16 0.70710678118654752440084436210485f
#define TWIDDLE_1_8 COS_PI_4_16, -COS_PI_4_16
#define TWIDDLE_3_8 -COS_PI_4_16, -COS_PI_4_16

///////////////////////////////////////////////////////////////////////////////
// Common types
///////////////////////////////////////////////////////////////////////////////

struct change_percall
{
    unsigned int thread_count;
    unsigned int ostride;
    unsigned int istride;
    unsigned int pstride;
    float phase_base;
};

class csfft512x512_plan
{
public:
    // Constructor
    csfft512x512_plan(UINT slices, accelerator_view av)
        : m_slices(slices), 
        m_av(av),
        m_tmp_array_view(array<float_2>((MAX_BUF_LEN / sizeof(float_2)), av))
    {
        create_input();
    }

    void fft_512x512_c2c_amp(array_view<float_2> dst, array_view<const float_2> src);

private:
    void radix008A_amp(array_view<float_2> dst,
        array_view<const float_2> src,
        UINT thread_count,
        UINT istride,
        change_percall change_percall);

    static void ft2(float_2 &a, float_2 &b) restrict(amp);
    static void cmul_forward(float_2 &a, float bx, float by) restrict(amp);
    static void upd_forward(float_2 &a, float_2 &b) restrict(amp);
    static void fft_forward_4(float_2 complex_num[8]) restrict(amp);
    static void fft_forward_8(float_2 complex_num[8]) restrict(amp);
    static void twiddle(float_2 &d, float phase) restrict(amp);
    static void twiddle_8(float_2 complex_num[8], float phase) restrict(amp);

    void create_input();

    UINT m_slices;

    // For 512x512 config, we need 6 buffers
    change_percall m_change_percall[6];

    // Temp data
    array_view<float_2> m_tmp_array_view;

    accelerator_view m_av;
};

#endif
