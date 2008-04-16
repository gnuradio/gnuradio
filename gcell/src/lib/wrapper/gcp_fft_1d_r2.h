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
 * \brief Submit a job that computes the forward or reverse FFT.
 *
 * \param mgr is the job manager instance
 * \param log2_fft_length is the log2 of the fft_length (4 <= x <= 13).
 * \param forward is true to compute the forward xform
 * \param out is the fft_length output from FFT (must be 16-byte aligned).
 * \param in is the fft_length input to FFT (must be 16-byte aligned).
 * \param W is fft_length/4 twiddle factor input to FFT (must be 16-byte aligned).
 *
 * Returns a job descriptor which should be passed to wait_job*.
 * Throws an exception in the event of a problem.
 */
gc_job_desc *
gcp_fft_1d_r2_submit(gc_job_manager_sptr mgr,
		     unsigned int log2_fft_length,
		     bool forward,
		     std::complex<float> *out,
		     const std::complex<float> *in,
		     const std::complex<float> *W);

/*!
 * \brief Compute twiddle factors for forward transform.
 *
 * \param log2_fft_length is the log2 of the fft_length.
 * \param W is fft_length/4 twiddle factor output (must be 16-byte aligned).
 */
void
gcp_fft_1d_r2_forward_twiddle(unsigned int log2_fft_length, std::complex<float> *W);

/*!
 * \brief Compute twiddle factors for reverse transform.
 *
 * \param log2_fft_length is the log2 of the fft_length.
 * \param W is fft_length/4 twiddle factor output (must be 16-byte aligned).
 */
void
gcp_fft_1d_r2_reverse_twiddle(unsigned int log2_fft_length, std::complex<float> *W);

#endif /* INCLUDED_GCP_FFT_1D_R2_H */
