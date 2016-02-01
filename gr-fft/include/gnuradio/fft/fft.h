/* -*- c++ -*- */
/*
 * Copyright 2003,2008,2012 Free Software Foundation, Inc.
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

#ifndef _FFT_FFT_H_
#define _FFT_FFT_H_

/*
 * Wrappers for FFTW single precision 1d dft
 */

#include <gnuradio/fft/api.h>
#include <gnuradio/gr_complex.h>
#include <boost/thread.hpp>

namespace gr {
  namespace fft {


    /*! \brief Helper function for allocating complex* buffers
     */
    FFT_API gr_complex* malloc_complex(int size);

    /*! \brief Helper function for allocating float* buffers
     */
    FFT_API float* malloc_float(int size);

    /*! \brief Helper function for allocating double* buffers
     */
    FFT_API double* malloc_double(int size);

    /*! \brief Helper function for freeing fft buffers
     */
    FFT_API void free(void *b);

    /*!
     * \brief Export reference to planner mutex for those apps that
     * want to use FFTW w/o using the fft_impl_fftw* classes.
     */
    class FFT_API planner {
    public:
      typedef boost::mutex::scoped_lock scoped_lock;
      /*!
       * Return reference to planner mutex
       */
      static boost::mutex &mutex();
    };

    /*!
     * \brief FFT: complex in, complex out
     * \ingroup misc
     */
    class FFT_API fft_complex {
      int         d_fft_size;
      int         d_nthreads;
      gr_complex *d_inbuf;
      gr_complex *d_outbuf;
      void       *d_plan;

    public:
      fft_complex(int fft_size, bool forward = true, int nthreads=1);
      virtual ~fft_complex();

      /*
       * These return pointers to buffers owned by fft_impl_fft_complex
       * into which input and output take place. It's done this way in
       * order to ensure optimal alignment for SIMD instructions.
       */
      gr_complex *get_inbuf()  const { return d_inbuf; }
      gr_complex *get_outbuf() const { return d_outbuf; }

      int inbuf_length()  const { return d_fft_size; }
      int outbuf_length() const { return d_fft_size; }

      /*!
       *  Set the number of threads to use for caclulation.
       */
      void set_nthreads(int n);

      /*!
       *  Get the number of threads being used by FFTW
       */
      int nthreads() const { return d_nthreads; }

      /*!
       * compute FFT. The input comes from inbuf, the output is placed in
       * outbuf.
       */
      void execute();
    };

    /*!
     * \brief FFT: real in, complex out
     * \ingroup misc
     */
    class FFT_API fft_real_fwd {
      int     d_fft_size;
      int         d_nthreads;
      float  *d_inbuf;
      gr_complex *d_outbuf;
      void   *d_plan;

    public:
      fft_real_fwd (int fft_size, int nthreads=1);
      virtual ~fft_real_fwd ();

      /*
       * These return pointers to buffers owned by fft_impl_fft_real_fwd
       * into which input and output take place. It's done this way in
       * order to ensure optimal alignment for SIMD instructions.
       */
      float *get_inbuf()      const { return d_inbuf; }
      gr_complex *get_outbuf() const { return d_outbuf; }

      int inbuf_length()  const { return d_fft_size; }
      int outbuf_length() const { return d_fft_size / 2 + 1; }

      /*!
       *  Set the number of threads to use for caclulation.
       */
      void set_nthreads(int n);

      /*!
       *  Get the number of threads being used by FFTW
       */
      int nthreads() const { return d_nthreads; }

      /*!
       * compute FFT. The input comes from inbuf, the output is placed in
       * outbuf.
       */
      void execute();
    };

    /*!
     * \brief FFT: complex in, float out
     * \ingroup misc
     */
    class FFT_API fft_real_rev {
      int         d_fft_size;
      int         d_nthreads;
      gr_complex *d_inbuf;
      float      *d_outbuf;
      void       *d_plan;

    public:
      fft_real_rev(int fft_size, int nthreads=1);
      virtual ~fft_real_rev();

      /*
       * These return pointers to buffers owned by fft_impl_fft_real_rev
       * into which input and output take place. It's done this way in
       * order to ensure optimal alignment for SIMD instructions.
       */
      gr_complex *get_inbuf() const { return d_inbuf; }
      float *get_outbuf() const { return d_outbuf; }

      int inbuf_length()  const { return d_fft_size / 2 + 1; }
      int outbuf_length() const { return d_fft_size; }

      /*!
       *  Set the number of threads to use for caclulation.
       */
      void set_nthreads(int n);

      /*!
       *  Get the number of threads being used by FFTW
       */
      int nthreads() const { return d_nthreads; }

      /*!
       * compute FFT. The input comes from inbuf, the output is placed in
       * outbuf.
       */
      void execute();
    };

  } /* namespace fft */
} /*namespace gr */

#endif /* _FFT_FFT_H_ */
