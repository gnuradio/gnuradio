/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#include <digital_api.h>
#include <gr_hier_block2.h>
#include <gr_char_to_float.h>
#include <gr_interp_fir_filter_fff.h>
#include <gr_frequency_modulator_fc.h>
#include <gr_cpm.h>


class digital_cpmmod_bc;
typedef boost::shared_ptr<digital_cpmmod_bc> digital_cpmmod_bc_sptr;


DIGITAL_API digital_cpmmod_bc_sptr
digital_make_cpmmod_bc(int type, float h,
		       unsigned samples_per_sym,
		       unsigned L, double beta=0.3);

/*!
 * \brief Generic CPM modulator
 *
 * \ingroup modulation_blk
 * \ingroup digital
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
 *
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
class DIGITAL_API digital_cpmmod_bc : public gr_hier_block2
{
  friend DIGITAL_API digital_cpmmod_bc_sptr digital_make_cpmmod_bc(int type, float h,
						       unsigned samples_per_sym,
						       unsigned L, double beta);
  
  std::vector<float> d_taps;
  gr_char_to_float_sptr d_char_to_float;
  gr_interp_fir_filter_fff_sptr d_pulse_shaper;
  gr_frequency_modulator_fc_sptr d_fm;
  
protected:
  digital_cpmmod_bc(gr_cpm::cpm_type type, float h,
		    unsigned samples_per_sym,
		    unsigned L, double beta);
  
public:
  //! Return the phase response FIR taps
  std::vector<float> get_taps() { return d_taps; };
};

#endif /* INCLUDED_DIGITAL_CPMMOD_BC_H */

