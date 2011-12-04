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

// Defined during configure; avoids trying to locate
// header files if SHD was not installed.
#ifdef GR_HAVE_SHD

#define GR_SHD_API

//load generated python docstrings
%include "shd_swig_doc.i"

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

////////////////////////////////////////////////////////////////////////
// block headers
////////////////////////////////////////////////////////////////////////
%{
#include <gr_shd_smini_source.h>
#include <gr_shd_smini_sink.h>
%}

////////////////////////////////////////////////////////////////////////
// used types
////////////////////////////////////////////////////////////////////////
%template(string_vector_t) std::vector<std::string>;

%include <shd/config.hpp>

%include <shd/utils/pimpl.hpp>

%ignore shd::dict::operator[]; //ignore warnings about %extend
%include <shd/types/dict.hpp>
%template(string_string_dict_t) shd::dict<std::string, std::string>; //define after dict

%include <shd/types/device_addr.hpp>

%include <shd/types/io_type.hpp>

%template(range_vector_t) std::vector<shd::range_t>; //define before range
%include <shd/types/ranges.hpp>

%include <shd/types/tune_request.hpp>

%include <shd/types/tune_result.hpp>

%include <shd/types/io_type.hpp>

%include <shd/types/time_spec.hpp>

%include <shd/types/clock_config.hpp>

%include <shd/types/metadata.hpp>

%ignore shd::device::register_device; //causes compile to choke in MSVC
%include <shd/device.hpp>
%template(device_addr_vector_t) std::vector<shd::device_addr_t>;

%include <shd/types/sensors.hpp>

////////////////////////////////////////////////////////////////////////
// swig dboard_iface for python access
////////////////////////////////////////////////////////////////////////
%include stdint.i
%include <shd/types/serial.hpp>
%template(byte_vector_t) std::vector<uint8_t>;
%include <shd/xmini/dboard_iface.hpp>

%template(dboard_iface_sptr) boost::shared_ptr<shd::xmini::dboard_iface>;

////////////////////////////////////////////////////////////////////////
// block magic
////////////////////////////////////////////////////////////////////////
GR_SWIG_BLOCK_MAGIC(shd,smini_source)
%include <gr_shd_smini_source.h>

GR_SWIG_BLOCK_MAGIC(shd,smini_sink)
%include <gr_shd_smini_sink.h>

////////////////////////////////////////////////////////////////////////
// helpful constants
////////////////////////////////////////////////////////////////////////
%{
static const size_t ALL_MBOARDS = shd::xmini::multi_xmini::ALL_MBOARDS;
%}
static const size_t ALL_MBOARDS;

#if SWIGGUILE
%scheme %{
(load-extension-global "libguile-gnuradio-shd_swig" "scm_init_gnuradio_shd_swig_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif  /* SWIGGUILE */

#endif  /* GR_HAVE_SHD */
