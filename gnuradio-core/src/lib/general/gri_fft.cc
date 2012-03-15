/* -*- c++ -*- */
/*
 * Copyright 2003,2008,2011 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <gri_fft.h>
#include <gr_sys_paths.h>
#include <fftw3.h>

#ifdef _MSC_VER //http://www.fftw.org/install/windows.html#DLLwisdom
static void my_fftw_write_char(char c, void *f) { fputc(c, (FILE *) f); }
#define fftw_export_wisdom_to_file(f) fftw_export_wisdom(my_fftw_write_char, (void*) (f))
#define fftwf_export_wisdom_to_file(f) fftwf_export_wisdom(my_fftw_write_char, (void*) (f))
#define fftwl_export_wisdom_to_file(f) fftwl_export_wisdom(my_fftw_write_char, (void*) (f))

static int my_fftw_read_char(void *f) { return fgetc((FILE *) f); }
#define fftw_import_wisdom_from_file(f) fftw_import_wisdom(my_fftw_read_char, (void*) (f))
#define fftwf_import_wisdom_from_file(f) fftwf_import_wisdom(my_fftw_read_char, (void*) (f))
#define fftwl_import_wisdom_from_file(f) fftwl_import_wisdom(my_fftw_read_char, (void*) (f))
#endif //_MSC_VER

#include <gr_complex.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cassert>
#include <stdexcept>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

gr_complex *
gri_fft_malloc_complex(int size)
{
  return (gr_complex*)fftwf_malloc(sizeof(gr_complex)*size);
}

float *
gri_fft_malloc_float(int size)
{
  return (float*)fftwf_malloc(sizeof(float)*size);
}

void
gri_fft_free(void *b)
{
  fftwf_free(b);
}

boost::mutex &
gri_fft_planner::mutex()
{
  static boost::mutex  s_planning_mutex;

  return s_planning_mutex;
}

static const char *
wisdom_filename ()
{
  static fs::path path;
  path = fs::path(gr_appdata_path()) / ".gr_fftw_wisdom";
  return path.string().c_str();
}

static void 
gri_fftw_import_wisdom ()
{
  const char *filename = wisdom_filename ();
  FILE *fp = fopen (filename, "r");
  if (fp != 0){
    int r = fftwf_import_wisdom_from_file (fp);
    fclose (fp);
    if (!r){
      fprintf (stderr, "gri_fftw: can't import wisdom from %s\n", filename);
    }
  }
}

static void
gri_fftw_config_threading (int nthreads)
{
  static int fftw_threads_inited = 0;
  
#ifdef FFTW3F_THREADS
  if (fftw_threads_inited == 0)
  {
	  fftw_threads_inited = 1;
	  fftwf_init_threads();
  }
  
  fftwf_plan_with_nthreads(nthreads);
#endif
}

static void
gri_fftw_export_wisdom ()
{
  const char *filename = wisdom_filename ();
  FILE *fp = fopen (filename, "w");
  if (fp != 0){
    fftwf_export_wisdom_to_file (fp);
    fclose (fp);
  }
  else {
    fprintf (stderr, "gri_fftw: ");
    perror (filename);
  }
}

// ----------------------------------------------------------------

gri_fft_complex::gri_fft_complex (int fft_size, bool forward, int nthreads)
{
  // Hold global mutex during plan construction and destruction.
  gri_fft_planner::scoped_lock	lock(gri_fft_planner::mutex());

  assert (sizeof (fftwf_complex) == sizeof (gr_complex));
  
  if (fft_size <= 0)
    throw std::out_of_range ("gri_fftw: invalid fft_size");
  
  d_fft_size = fft_size;
  d_inbuf = (gr_complex *) fftwf_malloc (sizeof (gr_complex) * inbuf_length ());
  if (d_inbuf == 0)
    throw std::runtime_error ("fftwf_malloc");
  
  d_outbuf = (gr_complex *) fftwf_malloc (sizeof (gr_complex) * outbuf_length ());
  if (d_outbuf == 0){
    fftwf_free (d_inbuf);
    throw std::runtime_error ("fftwf_malloc");
  }

  d_nthreads = nthreads;
  gri_fftw_config_threading (nthreads);
  gri_fftw_import_wisdom ();	// load prior wisdom from disk

  d_plan = fftwf_plan_dft_1d (fft_size,
			      reinterpret_cast<fftwf_complex *>(d_inbuf), 
			      reinterpret_cast<fftwf_complex *>(d_outbuf),
			      forward ? FFTW_FORWARD : FFTW_BACKWARD,
			      FFTW_MEASURE);

  if (d_plan == NULL) {
    fprintf(stderr, "gri_fft_complex: error creating plan\n");
    throw std::runtime_error ("fftwf_plan_dft_1d failed");
  }
  gri_fftw_export_wisdom ();	// store new wisdom to disk
}

gri_fft_complex::~gri_fft_complex ()
{
  // Hold global mutex during plan construction and destruction.
  gri_fft_planner::scoped_lock	lock(gri_fft_planner::mutex());

  fftwf_destroy_plan ((fftwf_plan) d_plan);
  fftwf_free (d_inbuf);
  fftwf_free (d_outbuf);
}

void 
gri_fft_complex::set_nthreads(int n)
{
  if (n <= 0)
    throw std::out_of_range ("gri_fftw: invalid number of threads");
  d_nthreads = n;

#ifdef FFTW3F_THREADS
  fftwf_plan_with_nthreads(d_nthreads);
#endif
}

void
gri_fft_complex::execute ()
{
  fftwf_execute ((fftwf_plan) d_plan);
}

// ----------------------------------------------------------------

gri_fft_real_fwd::gri_fft_real_fwd (int fft_size, int nthreads)
{
  // Hold global mutex during plan construction and destruction.
  gri_fft_planner::scoped_lock	lock(gri_fft_planner::mutex());

  assert (sizeof (fftwf_complex) == sizeof (gr_complex));
  
  if (fft_size <= 0)
    throw std::out_of_range ("gri_fftw: invalid fft_size");
  
  d_fft_size = fft_size;
  d_inbuf = (float *) fftwf_malloc (sizeof (float) * inbuf_length ());
  if (d_inbuf == 0)
    throw std::runtime_error ("fftwf_malloc");
  
  d_outbuf = (gr_complex *) fftwf_malloc (sizeof (gr_complex) * outbuf_length ());
  if (d_outbuf == 0){
    fftwf_free (d_inbuf);
    throw std::runtime_error ("fftwf_malloc");
  }

  d_nthreads = nthreads;
  gri_fftw_config_threading (nthreads);
  gri_fftw_import_wisdom ();	// load prior wisdom from disk
  
  d_plan = fftwf_plan_dft_r2c_1d (fft_size,
				  d_inbuf,
				  reinterpret_cast<fftwf_complex *>(d_outbuf),
				  FFTW_MEASURE);

  if (d_plan == NULL) {
    fprintf(stderr, "gri_fft_real_fwd: error creating plan\n");
    throw std::runtime_error ("fftwf_plan_dft_r2c_1d failed");
  }
  gri_fftw_export_wisdom ();	// store new wisdom to disk
}

gri_fft_real_fwd::~gri_fft_real_fwd ()
{
  // Hold global mutex during plan construction and destruction.
  gri_fft_planner::scoped_lock	lock(gri_fft_planner::mutex());

  fftwf_destroy_plan ((fftwf_plan) d_plan);
  fftwf_free (d_inbuf);
  fftwf_free (d_outbuf);
}

void 
gri_fft_real_fwd::set_nthreads(int n)
{
  if (n <= 0)
    throw std::out_of_range ("gri_fftw: invalid number of threads");
  d_nthreads = n;

#ifdef FFTW3F_THREADS
  fftwf_plan_with_nthreads(d_nthreads);
#endif
}

void
gri_fft_real_fwd::execute ()
{
  fftwf_execute ((fftwf_plan) d_plan);
}

// ----------------------------------------------------------------

gri_fft_real_rev::gri_fft_real_rev (int fft_size, int nthreads)
{
  // Hold global mutex during plan construction and destruction.
  gri_fft_planner::scoped_lock	lock(gri_fft_planner::mutex());

  assert (sizeof (fftwf_complex) == sizeof (gr_complex));
  
  if (fft_size <= 0)
    throw std::out_of_range ("gri_fftw: invalid fft_size");
  
  d_fft_size = fft_size;
  d_inbuf = (gr_complex *) fftwf_malloc (sizeof (gr_complex) * inbuf_length ());
  if (d_inbuf == 0)
    throw std::runtime_error ("fftwf_malloc");
  
  d_outbuf = (float *) fftwf_malloc (sizeof (float) * outbuf_length ());
  if (d_outbuf == 0){
    fftwf_free (d_inbuf);
    throw std::runtime_error ("fftwf_malloc");
  }

  d_nthreads = nthreads;
  gri_fftw_config_threading (nthreads);
  gri_fftw_import_wisdom ();	// load prior wisdom from disk
  
  // FIXME If there's ever a chance that the planning functions
  // will be called in multiple threads, we've got to ensure single
  // threaded access.  They are not thread-safe.
  d_plan = fftwf_plan_dft_c2r_1d (fft_size,
				  reinterpret_cast<fftwf_complex *>(d_inbuf),
				  d_outbuf,
				  FFTW_MEASURE);

  if (d_plan == NULL) {
    fprintf(stderr, "gri_fft_real_rev: error creating plan\n");
    throw std::runtime_error ("fftwf_plan_dft_c2r_1d failed");
  }
  gri_fftw_export_wisdom ();	// store new wisdom to disk
}

gri_fft_real_rev::~gri_fft_real_rev ()
{
  fftwf_destroy_plan ((fftwf_plan) d_plan);
  fftwf_free (d_inbuf);
  fftwf_free (d_outbuf);
}

void 
gri_fft_real_rev::set_nthreads(int n)
{
  if (n <= 0)
    throw std::out_of_range ("gri_fftw: invalid number of threads");
  d_nthreads = n;

#ifdef FFTW3F_THREADS
  fftwf_plan_with_nthreads(d_nthreads);
#endif
}

void
gri_fft_real_rev::execute ()
{
  fftwf_execute ((fftwf_plan) d_plan);
}

