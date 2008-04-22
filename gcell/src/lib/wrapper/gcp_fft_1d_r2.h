/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_GCP_FFT_1D_R2_H
#define INCLUDED_GCP_FFT_1D_R2_H

#include <gc_job_manager.h>
#include <complex>

/*!
 * \brief Submit a job that computes the forward or inverse FFT.
 *
 * \param mgr is the job manager instance
 * \param log2_fft_length is the log2 of the fft_length (4 <= x <= 12).
 * \param forward is true to compute the forward transform, else the inverse.
 * \param shift indicates if an "fftshift" should be applied to the output data
 * \param out is the fft_length output from FFT (must be 16-byte aligned).
 * \param in is the fft_length input to FFT (must be 16-byte aligned).
 * \param twiddle is fft_length/4 twiddle factor input to FFT (must be 16-byte aligned).
 * \param window is the window to be applied to the input data.
 *    The window length must be either 0 or fft_length (must be 16-byte aligned).
 *
 * Returns a shared_ptr to a job descriptor which should be passed to wait_job*.
 * Throws an exception in the event of a problem.
 * This uses the FFTW conventions for scaling.  That is, neither the forward nor inverse
 * are scaled by 1/fft_length.
 */
gc_job_desc_sptr
gcp_fft_1d_r2_submit(gc_job_manager_sptr mgr,
		     unsigned int log2_fft_length,
		     bool forward,
		     bool shift,
		     std::complex<float> *out,
		     const std::complex<float> *in,
		     const std::complex<float> *twiddle,
		     const float *window);

/*!
 * \brief Compute twiddle factors 
 *
 * \param log2_fft_length is the log2 of the fft_length.
 * \param W is fft_length/4 twiddle factor output (must be 16-byte aligned).
 */
void
gcp_fft_1d_r2_twiddle(unsigned int log2_fft_length, std::complex<float> *W);

#endif /* INCLUDED_GCP_FFT_1D_R2_H */
