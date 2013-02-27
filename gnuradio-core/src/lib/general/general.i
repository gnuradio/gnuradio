/* -*- c++ -*- */
/*
 * Copyright 2004-2012 Free Software Foundation, Inc.
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

%{

#include <gri_control_loop.h>
#include <gr_nop.h>
#include <gr_null_sink.h>
#include <gr_null_source.h>
#include <gr_head.h>
#include <gr_skiphead.h>
#include <gr_remez.h>
#include <gr_check_counting_s.h>
#include <gr_lfsr_32k_source_s.h>
#include <gr_check_lfsr_32k_s.h>
#include <gr_align_on_samplenumbers_ss.h>
#include <gr_complex_to_xxx.h>
#include <gr_complex_to_interleaved_short.h>
//#include <gr_endianness.h>
#include <gr_endian_swap.h>
#include <gr_firdes.h>
#include <gr_random_pdu.h>
#include <gr_fake_channel_coder_pp.h>
#include <gr_vco_f.h>
#include <gr_pa_2x2_phase_combiner.h>
#include <gr_kludge_copy.h>
#include <gr_prefs.h>
#include <gr_constants.h>
#include <gr_test_types.h>
#include <gr_test.h>
#include <gr_feval.h>
#include <gr_bin_statistics_f.h>
#include <gr_copy.h>
#include <complex_vec_test.h>
#include <gr_annotator_alltoall.h>
#include <gr_annotator_1to1.h>
#include <gr_annotator_raw.h>
#include <gr_burst_tagger.h>
#include <gr_vector_map.h>
#include <gr_tag_debug.h>
#include <gr_message_strobe.h>
%}

%include "gri_control_loop.i"
%include "gr_nop.i"
%include "gr_null_sink.i"
%include "gr_null_source.i"
%include "gr_head.i"
%include "gr_skiphead.i"
%include "gr_remez.i"
%include "gr_check_counting_s.i"
%include "gr_lfsr_32k_source_s.i"
%include "gr_check_lfsr_32k_s.i"
%include "gr_align_on_samplenumbers_ss.i"
%include "gr_complex_to_xxx.i"
%include "gr_complex_to_interleaved_short.i"
//%include "gr_endianness.i"
%include "gr_endian_swap.i"
%include "gr_firdes.i"
%include "gr_random_pdu.i"
%include "gr_fake_channel_coder_pp.i"
%include "gr_vco_f.i"
%include "gr_pa_2x2_phase_combiner.i"
%include "gr_kludge_copy.i"
%include "gr_prefs.i"
%include "gr_constants.i"
%include "gr_test_types.h"
%include "gr_test.i"
%include "gr_feval.i"
%include "gr_bin_statistics_f.i"
%include "gr_copy.i"
%include "complex_vec_test.i"
%include "gr_annotator_alltoall.i"
%include "gr_annotator_1to1.i"
%include "gr_annotator_raw.i"
%include "gr_burst_tagger.i"
%include "gr_vector_map.i"
%include "gr_tag_debug.i"
%include "gr_block_gateway.i"
%include "gr_message_strobe.i"


#ifdef GR_CTRLPORT

%{
#include <gr_ctrlport_probe_c.h>
#include <gr_ctrlport_probe2_c.h>
%}

%include "gr_ctrlport_probe_c.i"
%include "gr_ctrlport_probe2_c.i"

#endif /* GR_CTRLPORT */
