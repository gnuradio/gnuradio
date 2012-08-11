/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#define DIGITAL_API

%include "gnuradio.i"

//load generated python docstrings
%include "digital_swig_doc.i"

//#if SWIGPYTHON
//enum snr_est_type_t {
//  SNR_EST_SIMPLE = 0,	// Simple estimator (>= 7 dB)
//  SNR_EST_SKEW,	        // Skewness-base est (>= 5 dB)
//  SNR_EST_M2M4,	        // 2nd & 4th moment est (>= 1 dB)
//  SNR_EST_SVR           // SVR-based est (>= 0dB)
//};
//#endif

//%include <gri_control_loop.i>

// Used in the constellation objects
%template(unsigned_int_vector) std::vector<unsigned int>;

%{
#include "digital/additive_scrambler_bb.h"
#include "digital/binary_slicer_fb.h"
#include "digital/clock_recovery_mm_cc.h"
#include "digital/clock_recovery_mm_ff.h"
#include "digital/cma_equalizer_cc.h"
#include "digital/constellation.h"
#include "digital/constellation_receiver_cb.h"
#include "digital/constellation_decoder_cb.h"
%}

%include "digital/additive_scrambler_bb.h"
%include "digital/binary_slicer_fb.h"
%include "digital/clock_recovery_mm_cc.h"
%include "digital/clock_recovery_mm_ff.h"
%include "digital/cma_equalizer_cc.h"
%include "digital/constellation.h"
%include "digital/constellation_receiver_cb.h"
%include "digital/constellation_decoder_cb.h"

GR_SWIG_BLOCK_MAGIC2(digital, additive_scrambler_bb);
GR_SWIG_BLOCK_MAGIC2(digital, binary_slicer_fb);
GR_SWIG_BLOCK_MAGIC2(digital, clock_recovery_mm_cc);
GR_SWIG_BLOCK_MAGIC2(digital, clock_recovery_mm_ff);
GR_SWIG_BLOCK_MAGIC2(digital, cma_equalizer_cc);
GR_SWIG_BLOCK_MAGIC2(digital, constellation_receiver_cb);
GR_SWIG_BLOCK_MAGIC2(digital, constellation_decoder_cb);

// Properly package up constellation objects
%include "constellation.i"
