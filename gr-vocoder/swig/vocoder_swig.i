/* -*- c++ -*- */
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
%include "vocoder_swig_doc.i"

%include "vocoder_alaw_decode_bs.i"
%include "vocoder_alaw_encode_sb.i"
%include "vocoder_codec2_decode_ps.i"
%include "vocoder_codec2_encode_sp.i"
%include "vocoder_cvsd_decode_bs.i"
%include "vocoder_cvsd_encode_sb.i"
%include "vocoder_g721_decode_bs.i"
%include "vocoder_g721_encode_sb.i"
%include "vocoder_g723_24_decode_bs.i"
%include "vocoder_g723_24_encode_sb.i"
%include "vocoder_g723_40_decode_bs.i"
%include "vocoder_g723_40_encode_sb.i"
%include "vocoder_gsm_fr_decode_ps.i"
%include "vocoder_gsm_fr_encode_sp.i"
%include "vocoder_ulaw_decode_bs.i"
%include "vocoder_ulaw_encode_sb.i"

#if SWIGGUILE
%scheme %{
(load-extension-global "libguile-gnuradio-vocoder" "scm_init_gnuradio_gsm_vocoder_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif
