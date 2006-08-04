/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

%feature("autodoc","1");
%include "exception.i"
%import "gnuradio.i"

%{

#include "gnuradio_swig_bug_workaround.h"	// mandatory bug fix
#include <ecc_streams_encode_convolutional.h>
#include <ecc_streams_encode_turbo.h>
#include <ecc_metrics_decode_viterbi_full_block.h>
#include <ecc_syms_to_metrics.h>
#include <stdexcept>

%}

%include "ecc_streams_encode_convolutional.i"
%include "ecc_streams_encode_turbo.i"
%include "ecc_metrics_decode_viterbi_full_block.i"
%include "ecc_syms_to_metrics.i"
