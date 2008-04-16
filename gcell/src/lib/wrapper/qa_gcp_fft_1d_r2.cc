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

#include "qa_gcp_fft_1d_r2.h"
#include <cppunit/TestAssert.h>
#include <gcp_fft_1d_r2.h>
#include <fftw3.h>
#include <stdio.h>
#include <stdlib.h>	// random, posix_memalign
#include <algorithm>

typedef boost::shared_ptr<void> void_sptr;

// handle to embedded SPU executable
extern spe_program_handle_t gcell_all;

/*
 * Return pointer to cache-aligned chunk of storage of size size bytes.
 * Throw if can't allocate memory.  The storage should be freed
 * with "free" when done.  The memory is initialized to zero.
 */
static void *
aligned_alloc(size_t size, size_t alignment = 128)
{
  void *p = 0;
  if (posix_memalign(&p, alignment, size) != 0){
    perror("posix_memalign");
    throw std::runtime_error("memory");
  }
  memset(p, 0, size);		// zero the memory
  return p;
}

class free_deleter {
public:
  void operator()(void *p) {
    free(p);
  }
};

static boost::shared_ptr<void>
aligned_alloc_sptr(size_t size, size_t alignment = 128)
{
  return boost::shared_ptr<void>(aligned_alloc(size, alignment), free_deleter());
}

// test forward FFT
void
qa_gcp_fft_1d_r2::t1()
{
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_all);
  opts.nspes = 1;
  gc_job_manager_sptr mgr = gc_make_job_manager(&opts);

#if 1
  for (int log2_fft_size = 5; log2_fft_size <= 12; log2_fft_size++){
    test(mgr, log2_fft_size, true);
  }
#else
  test(mgr, 5, true);
#endif
}

// test reverse FFT
void
qa_gcp_fft_1d_r2::t2()
{
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_all);
  opts.nspes = 1;
  gc_job_manager_sptr mgr = gc_make_job_manager(&opts);

#if 1
  for (int log2_fft_size = 5; log2_fft_size <= 12; log2_fft_size++){
    test(mgr, log2_fft_size, false);
  }
#else
  test(mgr, 5, false);
#endif
}

void
qa_gcp_fft_1d_r2::t3()
{
}

void
qa_gcp_fft_1d_r2::t4()
{
}

static inline float
abs_diff(std::complex<float> x, std::complex<float> y)
{
  return std::max(std::abs(x.real()-y.real()),
		  std::abs(x.imag()-y.imag()));
}

static float
float_abs_rel_error(float ref, float actual)
{
  float delta = ref - actual;
  if (std::abs(ref) < 1e-18)
    ref = 1e-18;
  return std::abs(delta/ref);
}

static float
abs_rel_error(std::complex<float> ref, std::complex<float> actual)
{
  return std::max(float_abs_rel_error(ref.real(), actual.real()),
		  float_abs_rel_error(ref.imag(), actual.imag()));
}

void 
qa_gcp_fft_1d_r2::test(gc_job_manager_sptr mgr, int log2_fft_size, bool forward)
{
  int fft_size = 1 << log2_fft_size;

  // allocate aligned buffers with boost shared_ptr's
  void_sptr fftw_in_void = aligned_alloc_sptr(fft_size * sizeof(std::complex<float>), 128);
  void_sptr fftw_out_void = aligned_alloc_sptr(fft_size * sizeof(std::complex<float>), 128);
  void_sptr cell_in_void = aligned_alloc_sptr(fft_size * sizeof(std::complex<float>), 128);
  void_sptr cell_out_void = aligned_alloc_sptr(fft_size * sizeof(std::complex<float>), 128);
  void_sptr cell_twiddle_void = aligned_alloc_sptr(fft_size/4 * sizeof(std::complex<float>), 128);

  // cast them to the type we really want
  std::complex<float> *fftw_in = (std::complex<float> *) fftw_in_void.get();
  std::complex<float> *fftw_out = (std::complex<float> *) fftw_out_void.get();
  std::complex<float> *cell_in = (std::complex<float> *) cell_in_void.get();
  std::complex<float> *cell_out = (std::complex<float> *) cell_out_void.get();
  std::complex<float> *cell_twiddle = (std::complex<float> *) cell_twiddle_void.get();

  if (forward)
    gcp_fft_1d_r2_forward_twiddle(log2_fft_size, cell_twiddle);
  else
    gcp_fft_1d_r2_reverse_twiddle(log2_fft_size, cell_twiddle);

  srandom(1);		// we want reproducibility

  // initialize the input buffers
  for (int i = 0; i < fft_size; i++){
    std::complex<float> t((float) (random() & 0xfffff), (float) (random() & 0xfffff));
    fftw_in[i] = t;
    cell_in[i] = t;
  }

  // ------------------------------------------------------------------------
  // compute the reference answer
  fftwf_plan plan = fftwf_plan_dft_1d (fft_size,
				       reinterpret_cast<fftwf_complex *>(fftw_in), 
				       reinterpret_cast<fftwf_complex *>(fftw_out),
				       forward ? FFTW_FORWARD : FFTW_BACKWARD,
				       FFTW_ESTIMATE);
  if (plan == 0){
    fprintf(stderr, "qa_gcp_fft_1d_r2: error creating FFTW plan\n");
    throw std::runtime_error ("fftwf_plan_dft_r2c_1d failed");
  }
  
  fftwf_execute(plan);
  fftwf_destroy_plan(plan);

  // ------------------------------------------------------------------------
  // compute the answer on the cell
  gc_job_desc *jd = gcp_fft_1d_r2_submit(mgr, log2_fft_size, forward,
					 cell_out, cell_in, cell_twiddle);
  if (!mgr->wait_job(jd)){
    fprintf(stderr, "wait_job failed: %s\n", gc_job_status_string(jd->status).c_str());
    mgr->free_job_desc(jd);
    CPPUNIT_ASSERT(0);
  }
  mgr->free_job_desc(jd);

  // ------------------------------------------------------------------------
  // compute the maximum of the relative error
  float max_rel = 0.0;
  for (int i = 0; i < fft_size; i++){
    max_rel = std::max(max_rel, abs_rel_error(fftw_out[i], cell_out[i]));
    if (0)
      printf("(%16.3f, %16.3fj)  (%16.3f, %16.3fj)  (%16.3f, %16.3fj)\n",
	     fftw_out[i].real(), fftw_out[i].imag(),
	     cell_out[i].real(), cell_out[i].imag(),
	     fftw_out[i].real() - cell_out[i].real(),
	     fftw_out[i].imag() - cell_out[i].imag());
  }

  fprintf(stdout, "%s fft_size = %4d  max_rel_error = %e\n",
	  forward ? "fwd" : "rev", fft_size, max_rel);

  // CPPUNIT_ASSERT(max_rel <= 1e-4);

}
