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
#include <gcell/gcp_fft_1d_r2.h>
#include <stdint.h>
#include <stdexcept>
#include <math.h>

static void
init_jd(gc_job_desc *jd,
	gc_proc_id_t proc_id,
	unsigned log2_fft_length,
	bool shift,
	std::complex<float> *out,
	const std::complex<float> *in,
	const std::complex<float> *twiddle,
	const float *window)
{
  jd->proc_id = proc_id;
  jd->input.nargs = 2;
  jd->output.nargs = 0;
  jd->eaa.nargs = 4;

  jd->input.arg[0].u32 = log2_fft_length;
  jd->input.arg[1].u32 = shift;
  unsigned int fft_length = 1 << log2_fft_length;

  jd->eaa.arg[0].ea_addr = ptr_to_ea(out);
  jd->eaa.arg[0].direction = GCJD_DMA_PUT;
  jd->eaa.arg[0].put_size = sizeof(std::complex<float>) * fft_length;

  jd->eaa.arg[1].ea_addr = ptr_to_ea(const_cast<std::complex<float>*>(in));
  jd->eaa.arg[1].direction = GCJD_DMA_GET;
  jd->eaa.arg[1].get_size = sizeof(std::complex<float>) * fft_length;

  jd->eaa.arg[2].ea_addr = ptr_to_ea(const_cast<std::complex<float>*>(twiddle));
  jd->eaa.arg[2].direction = GCJD_DMA_GET;
  jd->eaa.arg[2].get_size = sizeof(std::complex<float>) * fft_length / 4;

  jd->eaa.arg[3].ea_addr = ptr_to_ea(const_cast<float*>(window));
  jd->eaa.arg[3].direction = GCJD_DMA_GET;
  if (window == 0)
    jd->eaa.arg[3].get_size = 0;
  else
    jd->eaa.arg[3].get_size = sizeof(float) * fft_length;
}

  
gc_job_desc_sptr
gcp_fft_1d_r2_submit(gc_job_manager_sptr mgr,
		     unsigned int log2_fft_length,
		     bool forward,
		     bool shift,
		     std::complex<float> *out,
		     const std::complex<float> *in,
		     const std::complex<float> *twiddle,
		     const float *window)
{
  unsigned int fft_length = 1 << log2_fft_length;
  if (fft_length > 4096)
    throw std::invalid_argument("fft_length > 4096");

  if ((intptr_t)out & 0xf)
    throw gc_bad_align("out");
  if ((intptr_t)in & 0xf)
    throw gc_bad_align("in");
  if ((intptr_t)twiddle & 0xf)
    throw gc_bad_align("twiddle");
  if ((intptr_t)window & 0xf)
    throw gc_bad_align("window");

  std::string proc_name;
  if (forward)
    proc_name = "fwd_fft_1d_r2";
  else
    proc_name = "inv_fft_1d_r2";

  gc_proc_id_t fft_id = mgr->lookup_proc(proc_name);
  gc_job_desc_sptr jd = gc_job_manager::alloc_job_desc(mgr);
  init_jd(jd.get(), fft_id, log2_fft_length, shift, out, in, twiddle, window);
  if (!mgr->submit_job(jd.get())){
    gc_job_status_t s = jd->status;
    throw gc_bad_submit(proc_name, s);
  }
  return jd;
}

void
gcp_fft_1d_r2_twiddle(unsigned int log2_fft_length, std::complex<float> *twiddle)
{
  unsigned int n = 1 << log2_fft_length;

  twiddle[0].real() = 1.0;
  twiddle[0].imag() = 0.0;
  for (unsigned i=1; i < n/4; i++){
    twiddle[i].real() =  cos(i * 2*M_PI/n);
    twiddle[n/4 - i].imag() = -twiddle[i].real();
  }
}
