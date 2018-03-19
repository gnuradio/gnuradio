/* -*- c++ -*- */
/*
 * Copyright 2003,2008,2011,2012 Free Software Foundation, Inc.
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

#include <gnuradio/fft/fft.h>
#include <gnuradio/sys_paths.h>
#include <gnuradio/gr_complex.h>
#include <volk/volk.h>
#include <fftw3.h>

#ifdef _WIN32 //http://www.fftw.org/install/windows.html#DLLwisdom
static void my_fftw_write_char(char c, void *f) { fputc(c, (FILE *) f); }
#define fftw_export_wisdom_to_file(f) fftw_export_wisdom(my_fftw_write_char, (void*) (f))
#define fftwf_export_wisdom_to_file(f) fftwf_export_wisdom(my_fftw_write_char, (void*) (f))
#define fftwl_export_wisdom_to_file(f) fftwl_export_wisdom(my_fftw_write_char, (void*) (f))

static int my_fftw_read_char(void *f) { return fgetc((FILE *) f); }
#define fftw_import_wisdom_from_file(f) fftw_import_wisdom(my_fftw_read_char, (void*) (f))
#define fftwf_import_wisdom_from_file(f) fftwf_import_wisdom(my_fftw_read_char, (void*) (f))
#define fftwl_import_wisdom_from_file(f) fftwl_import_wisdom(my_fftw_read_char, (void*) (f))
#include <fcntl.h> 
#include <io.h>
#define O_NOCTTY 0
#define O_NONBLOCK 0
#endif //_WIN32

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cassert>
#include <stdexcept>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
namespace fs = boost::filesystem;

namespace gr {
  namespace fft {
    static boost::mutex wisdom_thread_mutex;
    boost::interprocess::file_lock wisdom_lock;
    static bool wisdom_lock_init_done = false; // Modify while holding 'wisdom_thread_mutex'

    gr_complex *
    malloc_complex(int size)
    {
      return (gr_complex*) volk_malloc (sizeof (gr_complex)*size, volk_get_alignment ());
    }

    float *
    malloc_float(int size)
    {
      return (float*) volk_malloc (sizeof (float)*size, volk_get_alignment ());
    }

    double *
    malloc_double(int size)
    {
      return (double*) volk_malloc (sizeof (double)*size, volk_get_alignment ());
    }

    void
    free(void *b)
    {
      volk_free(b);
    }

    boost::mutex &
    planner::mutex()
    {
      static boost::mutex  s_planning_mutex;

      return s_planning_mutex;
    }

    static std::string
    wisdom_filename()
    {
      static fs::path path;
      path = fs::path(gr::appdata_path()) / ".gr_fftw_wisdom";
      return path.string();
    }

    static void
    wisdom_lock_init()
    {
      if (wisdom_lock_init_done)
        return;

      const std::string wisdom_lock_file = wisdom_filename() + ".lock";
      // std::cerr << "Creating FFTW wisdom lockfile: " << wisdom_lock_file << std::endl;
      int fd = open(wisdom_lock_file.c_str(),
        O_WRONLY | O_CREAT | O_NOCTTY | O_NONBLOCK,
        0666);
      if (fd < 0) {
        throw std::runtime_error("Failed to create FFTW wisdom lockfile: " + wisdom_lock_file);
      }
      close(fd);
      wisdom_lock = boost::interprocess::file_lock(wisdom_lock_file.c_str());
      wisdom_lock_init_done = true;
    }

    static void
    lock_wisdom()
    {
      wisdom_thread_mutex.lock();
      wisdom_lock_init();
      wisdom_lock.lock();
    }

    static void
    unlock_wisdom()
    {
      // Assumes 'lock_wisdom' has already been called (i.e. this file_lock is valid)
      wisdom_lock.unlock();
      wisdom_thread_mutex.unlock();
    }

    static void
    import_wisdom()
    {
      const std::string filename = wisdom_filename ();
      FILE *fp = fopen (filename.c_str(), "r");
      if (fp != 0){
        int r = fftwf_import_wisdom_from_file (fp);
        fclose (fp);
        if (!r){
          fprintf (stderr, "gr::fft: can't import wisdom from %s\n", filename.c_str());
        }
      }
    }

    static void
    config_threading(int nthreads)
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
    export_wisdom()
    {
      const std::string filename = wisdom_filename ();
      FILE *fp = fopen (filename.c_str(), "w");
      if (fp != 0){
        fftwf_export_wisdom_to_file (fp);
        fclose (fp);
      }
      else {
        fprintf (stderr, "fft_impl_fftw: ");
        perror (filename.c_str());
      }
    }

// ----------------------------------------------------------------

    fft_complex::fft_complex(int fft_size, bool forward, int nthreads)
    {
      // Hold global mutex during plan construction and destruction.
      planner::scoped_lock lock(planner::mutex());

      assert (sizeof (fftwf_complex) == sizeof (gr_complex));

      if (fft_size <= 0){
        throw std::out_of_range ("fft_impl_fftw: invalid fft_size");
      }

      d_fft_size = fft_size;
      d_inbuf = (gr_complex *) volk_malloc (sizeof (gr_complex) * inbuf_length (), volk_get_alignment ());
      if (d_inbuf == 0) {
        throw std::runtime_error ("volk_malloc");
      }
      d_outbuf = (gr_complex *) volk_malloc (sizeof (gr_complex) * outbuf_length (), volk_get_alignment ());
      if (d_outbuf == 0) {
        volk_free (d_inbuf);
        throw std::runtime_error ("volk_malloc");
      }

      d_nthreads = nthreads;
      config_threading(nthreads);
      lock_wisdom();
      import_wisdom();	// load prior wisdom from disk

      d_plan = fftwf_plan_dft_1d (fft_size,
                  reinterpret_cast<fftwf_complex *>(d_inbuf),
                  reinterpret_cast<fftwf_complex *>(d_outbuf),
                  forward ? FFTW_FORWARD : FFTW_BACKWARD,
                  FFTW_MEASURE);

      if (d_plan == NULL) {
        fprintf(stderr, "gr::fft: error creating plan\n");
        throw std::runtime_error ("fftwf_plan_dft_1d failed");
      }
      export_wisdom();	// store new wisdom to disk
      unlock_wisdom();
    }

    fft_complex::~fft_complex()
    {
      // Hold global mutex during plan construction and destruction.
      planner::scoped_lock lock(planner::mutex());

      fftwf_destroy_plan ((fftwf_plan) d_plan);
      volk_free (d_inbuf);
      volk_free (d_outbuf);
    }

    void
    fft_complex::set_nthreads(int n)
    {
      if (n <= 0) {
        throw std::out_of_range ("gr::fft: invalid number of threads");
      }
      d_nthreads = n;

#ifdef FFTW3F_THREADS
      fftwf_plan_with_nthreads(d_nthreads);
#endif
    }

    void
    fft_complex::execute()
    {
      fftwf_execute((fftwf_plan) d_plan);
    }

// ----------------------------------------------------------------

    fft_real_fwd::fft_real_fwd (int fft_size, int nthreads)
    {
      // Hold global mutex during plan construction and destruction.
      planner::scoped_lock lock(planner::mutex());

      assert (sizeof (fftwf_complex) == sizeof (gr_complex));

      if (fft_size <= 0) {
        throw std::out_of_range ("gr::fft: invalid fft_size");
      }

      d_fft_size = fft_size;
      d_inbuf = (float *) volk_malloc (sizeof (float) * inbuf_length (), volk_get_alignment ());
      if (d_inbuf == 0) {
        throw std::runtime_error ("volk_malloc");
      }

      d_outbuf = (gr_complex *) volk_malloc (sizeof (gr_complex) * outbuf_length (), volk_get_alignment ());
      if (d_outbuf == 0) {
        volk_free (d_inbuf);
        throw std::runtime_error ("volk_malloc");
      }

      d_nthreads = nthreads;
      config_threading(nthreads);
      lock_wisdom();
      import_wisdom();	// load prior wisdom from disk

      d_plan = fftwf_plan_dft_r2c_1d (fft_size,
                      d_inbuf,
                      reinterpret_cast<fftwf_complex *>(d_outbuf),
                      FFTW_MEASURE);

      if (d_plan == NULL) {
        fprintf(stderr, "gr::fft::fft_real_fwd: error creating plan\n");
        throw std::runtime_error ("fftwf_plan_dft_r2c_1d failed");
      }
      export_wisdom();	// store new wisdom to disk
      unlock_wisdom();
    }

    fft_real_fwd::~fft_real_fwd()
    {
      // Hold global mutex during plan construction and destruction.
      planner::scoped_lock lock(planner::mutex());

      fftwf_destroy_plan ((fftwf_plan) d_plan);
      volk_free (d_inbuf);
      volk_free (d_outbuf);
    }

    void
    fft_real_fwd::set_nthreads(int n)
    {
      if (n <= 0) {
        throw std::out_of_range ("gr::fft::fft_real_fwd::set_nthreads: invalid number of threads");
      }
      d_nthreads = n;

#ifdef FFTW3F_THREADS
      fftwf_plan_with_nthreads(d_nthreads);
#endif
    }

    void
    fft_real_fwd::execute()
    {
      fftwf_execute ((fftwf_plan) d_plan);
    }

    // ----------------------------------------------------------------

    fft_real_rev::fft_real_rev(int fft_size, int nthreads)
    {
      // Hold global mutex during plan construction and destruction.
      planner::scoped_lock lock(planner::mutex());

      assert (sizeof (fftwf_complex) == sizeof (gr_complex));

      if (fft_size <= 0) {
        throw std::out_of_range ("gr::fft::fft_real_rev: invalid fft_size");
      }

      d_fft_size = fft_size;
      d_inbuf = (gr_complex *) volk_malloc (sizeof (gr_complex) * inbuf_length (), volk_get_alignment ());
      if (d_inbuf == 0) {
        throw std::runtime_error ("volk_malloc");
      }

      d_outbuf = (float *) volk_malloc (sizeof (float) * outbuf_length (), volk_get_alignment ());
      if (d_outbuf == 0) {
        volk_free (d_inbuf);
        throw std::runtime_error ("volk_malloc");
      }

      d_nthreads = nthreads;
      config_threading(nthreads);
      lock_wisdom();
      import_wisdom();	// load prior wisdom from disk

      // FIXME If there's ever a chance that the planning functions
      // will be called in multiple threads, we've got to ensure single
      // threaded access.  They are not thread-safe.
      d_plan = fftwf_plan_dft_c2r_1d (fft_size,
                      reinterpret_cast<fftwf_complex *>(d_inbuf),
                      d_outbuf,
                      FFTW_MEASURE);

      if (d_plan == NULL) {
        fprintf(stderr, "gr::fft::fft_real_rev: error creating plan\n");
        throw std::runtime_error ("fftwf_plan_dft_c2r_1d failed");
      }
      export_wisdom ();	// store new wisdom to disk
      unlock_wisdom();
    }

    fft_real_rev::~fft_real_rev ()
    {
      // Hold global mutex during plan construction and destruction.
      planner::scoped_lock lock(planner::mutex());

      fftwf_destroy_plan ((fftwf_plan) d_plan);
      volk_free (d_inbuf);
      volk_free (d_outbuf);
    }

    void
    fft_real_rev::set_nthreads(int n)
    {
      if (n <= 0) {
        throw std::out_of_range ("gr::fft::fft_real_rev::set_nthreads: invalid number of threads");
      }
      d_nthreads = n;

#ifdef FFTW3F_THREADS
      fftwf_plan_with_nthreads(d_nthreads);
#endif
    }

    void
    fft_real_rev::execute ()
    {
      fftwf_execute ((fftwf_plan) d_plan);
    }

  } /* namespace fft */
} /* namespace gr */
