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

%include "gnuradio.i"

//load generated python docstrings
%include "digital_swig_doc.i"

%include <gri_control_loop.i>

%{
#include "digital_binary_slicer_fb.h"
#include "digital_clock_recovery_mm_cc.h"
#include "digital_clock_recovery_mm_ff.h"
#include "digital_cma_equalizer_cc.h"
#include "digital_constellation.h"
#include "digital_constellation_decoder_cb.h"
#include "digital_constellation_receiver_cb.h"
#include "digital_correlate_access_code_bb.h"
#include "digital_costas_loop_cc.h"
#include "digital_crc32.h"
#include "digital_fll_band_edge_cc.h"
#include "digital_kurtotic_equalizer_cc.h"
#include "digital_lms_dd_equalizer_cc.h"
#include "digital_mpsk_receiver_cc.h"
#include "digital_ofdm_cyclic_prefixer.h"
#include "digital_ofdm_frame_acquisition.h"
#include "digital_ofdm_frame_sink.h"
#include "digital_ofdm_insert_preamble.h"
#include "digital_ofdm_mapper_bcv.h"
#include "digital_ofdm_sampler.h"
#include "digital_cpmmod_bc.h"
#include "digital_gmskmod_bc.h"
%}

%include "digital_binary_slicer_fb.i"
%include "digital_clock_recovery_mm_cc.i"
%include "digital_clock_recovery_mm_ff.i"
%include "digital_cma_equalizer_cc.i"
%include "digital_constellation.i"
%include "digital_constellation_decoder_cb.i"
%include "digital_constellation_receiver_cb.i"
%include "digital_correlate_access_code_bb.i"
%include "digital_costas_loop_cc.i"
%include "digital_crc32.i"
%include "digital_fll_band_edge_cc.i"
%include "digital_kurtotic_equalizer_cc.i"
%include "digital_lms_dd_equalizer_cc.i"
%include "digital_mpsk_receiver_cc.i"
%include "digital_ofdm_cyclic_prefixer.i"
%include "digital_ofdm_frame_acquisition.i"
%include "digital_ofdm_frame_sink.i"
%include "digital_ofdm_insert_preamble.i"
%include "digital_ofdm_mapper_bcv.i"
%include "digital_ofdm_sampler.i"
%include "digital_cpmmod_bc.i"
%include "digital_gmskmod_bc.i"

#if SWIGGUILE
%scheme %{
(load-extension-global "libguile-gnuradio-digital_swig" "scm_init_gnuradio_digital_swig_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif
