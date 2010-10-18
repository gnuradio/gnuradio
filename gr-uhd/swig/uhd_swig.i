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

%include "gnuradio.i"
%include "std_string.i"
%include "std_vector.i"

namespace std {
    %template(StringVector) vector<string>;
}

%{
#include <uhd_mimo_source.h> //deprecated
#include <uhd_mimo_sink.h> //deprecated
#include <uhd_simple_source.h> //deprecated
#include <uhd_simple_sink.h> //deprecated

#include <uhd_multi_usrp_source.h>
#include <uhd_multi_usrp_sink.h>
#include <uhd_single_usrp_source.h>
#include <uhd_single_usrp_sink.h>
%}

%include <uhd/config.hpp>
%include <uhd/types/ranges.hpp>
%include <uhd/types/tune_result.hpp>
%include <uhd/types/io_type.hpp>
%include <uhd/types/time_spec.hpp>
%include <uhd/types/clock_config.hpp>

GR_SWIG_BLOCK_MAGIC(uhd,mimo_source) //deprecated
%include <uhd_mimo_source.h> //deprecated

GR_SWIG_BLOCK_MAGIC(uhd,mimo_sink) //deprecated
%include <uhd_mimo_sink.h> //deprecated

GR_SWIG_BLOCK_MAGIC(uhd,simple_source) //deprecated
%include <uhd_simple_source.h> //deprecated

GR_SWIG_BLOCK_MAGIC(uhd,simple_sink) //deprecated
%include <uhd_simple_sink.h> //deprecated

GR_SWIG_BLOCK_MAGIC(uhd,multi_usrp_source)
%include <uhd_multi_usrp_source.h>

GR_SWIG_BLOCK_MAGIC(uhd,multi_usrp_sink)
%include <uhd_multi_usrp_sink.h>

GR_SWIG_BLOCK_MAGIC(uhd,single_usrp_source)
%include <uhd_single_usrp_source.h>

GR_SWIG_BLOCK_MAGIC(uhd,single_usrp_sink)
%include <uhd_single_usrp_sink.h>
