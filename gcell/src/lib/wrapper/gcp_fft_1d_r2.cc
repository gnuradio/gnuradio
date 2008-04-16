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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gcp_fft_1d_r2.h>
#include <stdint.h>
#include <stdexcept>
#include <math.h>

static void
init_jd(gc_job_desc *jd,
	gc_proc_id_t proc_id,
	unsigned log2_fft_length,
	bool forward,
	std::complex<float> *out,
	const std::complex<float> *in,
	const std::complex<float> *W)
{
  jd->proc_id = proc_id;
  jd->input.nargs = 2;
  jd->output.nargs = 0;
  jd->eaa.nargs = 3;

  jd->input.arg[0].u32 = log2_fft_length;
  jd->input.arg[1].u32 = forward;
  unsigned int fft_length = 1 << log2_fft_length;

  jd->eaa.arg[0].ea_addr = ptr_to_ea(out);
  jd->eaa.arg[0].direction = GCJD_DMA_PUT;
  jd->eaa.arg[0].put_size = sizeof(std::complex<float>) * fft_length;

  jd->eaa.arg[1].ea_addr = ptr_to_ea(const_cast<std::complex<float>*>(in));
  jd->eaa.arg[1].direction = GCJD_DMA_GET;
  jd->eaa.arg[1].get_size = sizeof(std::complex<float>) * fft_length;

  jd->eaa.arg[2].ea_addr = ptr_to_ea(const_cast<std::complex<float>*>(W));
  jd->eaa.arg[2].direction = GCJD_DMA_GET;
  jd->eaa.arg[2].get_size = sizeof(std::complex<float>) * fft_length / 4;
}


gc_job_desc *
gcp_fft_1d_r2_submit(gc_job_manager_sptr mgr,
		     unsigned int log2_fft_length,
		     bool forward,
		     std::complex<float> *out,
		     const std::complex<float> *in,
		     const std::complex<float> *W)
{
  unsigned int fft_length = 1 << log2_fft_length;
  if (fft_length > 4096)
    throw std::invalid_argument("fft_length > 4096");

  if ((intptr_t)out & 0xf)
    throw gc_bad_align("out");
  if ((intptr_t)in & 0xf)
    throw gc_bad_align("in");
  if ((intptr_t)W & 0xf)
    throw gc_bad_align("W");

  gc_proc_id_t fft_id = mgr->lookup_proc("fft_1d_r2");
  gc_job_desc *jd = mgr->alloc_job_desc();
  init_jd(jd, fft_id, log2_fft_length, forward, out, in, W);
  if (!mgr->submit_job(jd)){
    gc_job_status_t s = jd->status;
    mgr->free_job_desc(jd);
    throw gc_bad_submit("fft_1d_r2", s);
  }
  return jd;
}

void
gcp_fft_1d_r2_twiddle(unsigned int log2_fft_length, std::complex<float> *W)
{
  unsigned int n = 1 << log2_fft_length;

  W[0].real() = 1.0;
  W[0].imag() = 0.0;
  for (unsigned i=1; i < n/4; i++){
    W[i].real() =  cos(i * 2*M_PI/n);
    W[n/4 - i].imag() = -W[i].real();
  }
}
