/*
 * Copyright 2016-2017 Free Software Foundation, Inc.
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
#define ANALOG_API
#define BLOCKS_API
#define FILTER_API

%include "gnuradio.i"
%include "stdint.i"

//load generated python docstrings
%include "digital_swig2_doc.i"

// %include "gnuradio/analog/cpm.h"

%{
#include <gnuradio/blocks/control_loop.h>
#include "gnuradio/digital/constellation.h"
#include "gnuradio/digital/timing_error_detector_type.h"
#include "gnuradio/digital/interpolating_resampler_type.h"
%}

%include <gnuradio/blocks/control_loop.h>
%include "gnuradio/digital/constellation.h"
%include "gnuradio/digital/timing_error_detector_type.h"
%include "gnuradio/digital/interpolating_resampler_type.h"

%{
#include "gnuradio/digital/mpsk_receiver_cc.h"
#include "gnuradio/digital/mpsk_snr_est.h"
#include "gnuradio/digital/mpsk_snr_est_cc.h"
#include "gnuradio/digital/header_format_base.h"
#include "gnuradio/digital/header_format_default.h"
#include "gnuradio/digital/header_format_counter.h"
#include "gnuradio/digital/header_format_crc.h"
#include "gnuradio/digital/header_format_ofdm.h"
#include "gnuradio/digital/protocol_formatter_async.h"
#include "gnuradio/digital/protocol_formatter_bb.h"
#include "gnuradio/digital/protocol_parser_b.h"
#include "gnuradio/digital/packet_header_default.h"
#include "gnuradio/digital/packet_header_ofdm.h"
#include "gnuradio/digital/packet_headergenerator_bb.h"
#include "gnuradio/digital/packet_headerparser_b.h"
#include "gnuradio/digital/packet_sink.h"
#include "gnuradio/digital/pfb_clock_sync_ccf.h"
#include "gnuradio/digital/pfb_clock_sync_fff.h"
#include "gnuradio/digital/pn_correlator_cc.h"
#include "gnuradio/digital/probe_density_b.h"
#include "gnuradio/digital/probe_mpsk_snr_est_c.h"
#include "gnuradio/digital/scrambler_bb.h"
#include "gnuradio/digital/simple_correlator.h"
#include "gnuradio/digital/simple_framer.h"
#include "gnuradio/digital/symbol_sync_cc.h"
#include "gnuradio/digital/symbol_sync_ff.h"
#include "gnuradio/digital/ofdm_serializer_vcc.h"
#include "gnuradio/digital/packet_headerparser_b.h"
#include "gnuradio/digital/header_payload_demux.h"
%}

%include "gnuradio/digital/mpsk_receiver_cc.h"
%include "gnuradio/digital/mpsk_snr_est.h"
%include "gnuradio/digital/mpsk_snr_est_cc.h"
%include "gnuradio/digital/header_format_base.h"
%include "gnuradio/digital/header_format_default.h"
%include "gnuradio/digital/header_format_counter.h"
%include "gnuradio/digital/header_format_crc.h"
%include "gnuradio/digital/header_format_ofdm.h"
%include "gnuradio/digital/protocol_formatter_async.h"
%include "gnuradio/digital/protocol_formatter_bb.h"
%include "gnuradio/digital/protocol_parser_b.h"
%include "gnuradio/digital/packet_header_default.h"
%include "gnuradio/digital/packet_header_ofdm.h"
%include "gnuradio/digital/packet_headergenerator_bb.h"
%include "gnuradio/digital/packet_headerparser_b.h"
%include "gnuradio/digital/packet_sink.h"
%include "gnuradio/digital/pfb_clock_sync_ccf.h"
%include "gnuradio/digital/pfb_clock_sync_fff.h"
%include "gnuradio/digital/pn_correlator_cc.h"
%include "gnuradio/digital/probe_density_b.h"
%include "gnuradio/digital/probe_mpsk_snr_est_c.h"
%include "gnuradio/digital/scrambler_bb.h"
%include "gnuradio/digital/simple_correlator.h"
%include "gnuradio/digital/simple_framer.h"
%include "gnuradio/digital/symbol_sync_cc.h"
%include "gnuradio/digital/symbol_sync_ff.h"

GR_SWIG_BLOCK_MAGIC2(digital, mpsk_receiver_cc);
GR_SWIG_BLOCK_MAGIC2(digital, mpsk_snr_est_cc);
GR_SWIG_BLOCK_MAGIC2(digital, protocol_formatter_async);
GR_SWIG_BLOCK_MAGIC2(digital, protocol_formatter_bb);
GR_SWIG_BLOCK_MAGIC2(digital, protocol_parser_b);
GR_SWIG_BLOCK_MAGIC2(digital, packet_headergenerator_bb);
GR_SWIG_BLOCK_MAGIC2(digital, packet_headerparser_b);
GR_SWIG_BLOCK_MAGIC2(digital, packet_sink);
GR_SWIG_BLOCK_MAGIC2(digital, pfb_clock_sync_ccf);
GR_SWIG_BLOCK_MAGIC2(digital, pfb_clock_sync_fff);
GR_SWIG_BLOCK_MAGIC2(digital, pn_correlator_cc);
GR_SWIG_BLOCK_MAGIC2(digital, probe_density_b);
GR_SWIG_BLOCK_MAGIC2(digital, probe_mpsk_snr_est_c);
GR_SWIG_BLOCK_MAGIC2(digital, scrambler_bb);
GR_SWIG_BLOCK_MAGIC2(digital, simple_correlator);
GR_SWIG_BLOCK_MAGIC2(digital, simple_framer);
GR_SWIG_BLOCK_MAGIC2(digital, symbol_sync_cc);
GR_SWIG_BLOCK_MAGIC2(digital, symbol_sync_ff);

// Properly package up non-block objects
%include "packet_header.i"
%include "constellation.i"
