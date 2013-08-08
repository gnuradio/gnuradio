/* -*- c++ -*- */
/*
 * Copyright 2010,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CPMMOD_BC_H
#define INCLUDED_DIGITAL_CPMMOD_BC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/hier_block2.h>
#include <gnuradio/analog/cpm.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Generic CPM modulator
     * \ingroup modulators_blk
     *
     * \details
     * Examples:
     * - Setting h = 0.5, L = 1, type = LREC yields MSK.
     * - Setting h = 0.5, type = GAUSSIAN and beta = 0.3 yields GMSK
     *           as used in GSM.
     *
     * The input of this block are symbols from an M-ary alphabet
     * +/-1, +/-3, ..., +/-(M-1). Usually, M = 2 and therefore, the
     * valid inputs are +/-1.
     * The modulator will silently accept any other inputs, though.
     * The output is the phase-modulated signal.
     */
    class DIGITAL_API cpmmod_bc : virtual public hier_block2
    {
    public:
      // gr::digital::cpmmod_bc::sptr
      typedef boost::shared_ptr<cpmmod_bc> sptr;

      /*!
       * Make CPM modulator block.
       *
       * \param type The modulation type. Can be one of LREC, LRC, LSRC, TFM
       *             or GAUSSIAN. See gr_cpm::phase_response() for a
       *             detailed description.
       * \param h The modulation index. \f$ h \cdot \pi\f$ is the maximum
       *          phase change that can occur between two symbols, i.e., if
       *          you only send ones, the phase will increase by \f$ h \cdot
       *          \pi\f$ every \p samples_per_sym samples. Set this to 0.5
       *          for Minimum Shift Keying variants.
       * \param samples_per_sym Samples per symbol.
       * \param L The length of the phase duration in symbols. For L=1, this
       *          yields full- response CPM symbols, for L > 1,
       *          partial-response.
       * \param beta For LSRC, this is the rolloff factor. For Gaussian
       *             pulses, this is the 3 dB time-bandwidth product.
       */
      static sptr make(analog::cpm::cpm_type type, float h,
		       int samples_per_sym,
		       int L, double beta=0.3);

      /*!
       * Make GMSK modulator block.
       *
       * The type is GAUSSIAN and the modulation index for GMSK is
       * 0.5. This are populated automatically by this factory
       * function.
       *
       * \param samples_per_sym Samples per symbol.
       * \param L The length of the phase duration in symbols. For L=1, this
       *          yields full- response CPM symbols, for L > 1,
       *          partial-response.
       * \param beta For LSRC, this is the rolloff factor. For Gaussian
       *             pulses, this is the 3 dB time-bandwidth product.
       */
      static sptr make_gmskmod_bc(int samples_per_sym=2,
				  int L=4, double beta=0.3);

      //! Return the phase response FIR taps
      virtual std::vector<float> taps() const = 0;

      //! Return the type of CPM modulator
      virtual int type() const = 0;

      //! Return the modulation index of the modulator.
      virtual float index() const = 0;

      //! Return the number of samples per symbol
      virtual int samples_per_sym() const = 0;

      //! Return the length of the phase duration (in symbols)
      virtual int length() const = 0;

      //! Return the value of beta for the modulator
      virtual double beta() const = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CPMMOD_BC_H */

