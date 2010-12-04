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

////////////////////////////////////////////////////////////////////////
// Language independent exception handler
////////////////////////////////////////////////////////////////////////
%include exception.i

%exception {
    try {
        $action
    }
    catch(std::exception &e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    }
    catch(...) {
        SWIG_exception(SWIG_RuntimeError, "Unknown exception");
    }

}

////////////////////////////////////////////////////////////////////////
// standard includes
////////////////////////////////////////////////////////////////////////
%include "gnuradio.i"

namespace std {
    %template(StringVector) vector<string>;
}

////////////////////////////////////////////////////////////////////////
// block headers
////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////
// used types
////////////////////////////////////////////////////////////////////////
%include <uhd/config.hpp>
%include <uhd/utils/pimpl.hpp>
%include <uhd/types/ranges.hpp>
%include <uhd/types/tune_request.hpp>
%include <uhd/types/tune_result.hpp>
%include <uhd/types/io_type.hpp>
%include <uhd/types/time_spec.hpp>
%include <uhd/types/clock_config.hpp>

//Re-create range typedefs here with %template as they are not imported.
//Replicate all the levels of templated inheritance so swig understands.

%template(float_range_t) uhd::range_t<float>;
%template(_float_range_vector_t) std::vector<uhd::range_t<float> >;
%template(gain_range_t) uhd::meta_range_t<float>;

%template(double_range_t) uhd::range_t<double>;
%template(_double_range_vector_t) std::vector<uhd::range_t<double> >;
%template(freq_range_t) uhd::meta_range_t<double>;

////////////////////////////////////////////////////////////////////////
// block magic
////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////
// helpful constants
////////////////////////////////////////////////////////////////////////
%{
static const size_t ALL_MBOARDS = uhd::usrp::multi_usrp::ALL_MBOARDS;
%}
static const size_t ALL_MBOARDS;

#if SWIGGUILE
%scheme %{
(load-extension "libguile-gnuradio-uhd_swig" "scm_init_gnuradio_uhd_swig_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif
