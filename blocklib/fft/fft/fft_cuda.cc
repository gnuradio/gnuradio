/* -*- c++ -*- */
/*
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "fft_cuda.h"
#include "fft_cuda_gen.h"


extern void exec_fft_shift(const cuFloatComplex* in,
                           cuFloatComplex* out,
                           int n,
                           int grid_size,
                           int block_size,
                           cudaStream_t stream);

namespace gr {
namespace fft {

template <class T, bool forward>
fft_cuda<T, forward>::fft_cuda(const typename fft<T, forward>::block_args& args)
    : INHERITED_CONSTRUCTORS(T, forward),
      d_fft_size(args.fft_size),
      d_shift(args.shift),
      d_fft(args.fft_size)
{
    if (args.window.empty() || args.window.size() == d_fft_size) {
        d_window = args.window;
    }
    else {
        throw std::runtime_error("fft: window not the same length as fft_size");
    }

    cudaStreamCreate(&d_stream);
}


template <>
void fft_cuda<gr_complex, true>::fft_and_shift(const gr_complex* in,
                                               gr_complex* out,
                                               int batch)
{
    int blockSize = 1024;
    int gridSize = (batch * d_fft_size + blockSize - 1) / blockSize;
    if (d_shift)
        exec_fft_shift((cuFloatComplex*)in,
                       (cuFloatComplex*)in,
                       batch * d_fft_size,
                       gridSize,
                       blockSize,
                       d_stream);
    d_fft.launch(in, out, batch, d_stream);
}

template <>
void fft_cuda<gr_complex, false>::fft_and_shift(const gr_complex* in,
                                                gr_complex* out,
                                                int batch)
{
    int blockSize = 1024;
    int gridSize = (batch * d_fft_size + blockSize - 1) / blockSize;
    if (d_shift)
        exec_fft_shift((cuFloatComplex*)in,
                       (cuFloatComplex*)in,
                       batch * d_fft_size,
                       gridSize,
                       blockSize,
                       d_stream);
    d_fft.launch(in, out, batch, d_stream);
}

template <>
void fft_cuda<float, true>::fft_and_shift(const float* in, gr_complex* out, int batch)
{
}

template <>
void fft_cuda<float, false>::fft_and_shift(const float* in, gr_complex* out, int batch)
{
}

template <class T, bool forward>
work_return_code_t fft_cuda<T, forward>::work(work_io& wio)
{

    auto in = wio.inputs()[0].items<T>();
    auto out = wio.outputs()[0].items<gr_complex>();
    auto noutput_items = wio.outputs()[0].n_items;

    fft_and_shift(in, out, noutput_items);
    cudaStreamSynchronize(d_stream);
    wio.produce_each(noutput_items);
    return work_return_code_t::WORK_OK;
}

} // namespace fft
} // namespace gr
