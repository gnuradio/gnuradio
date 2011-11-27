/* -*- c++ -*- */
/*
 * Copyright 2010-2011 Free Software Foundation, Inc.
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
// header files if UHD was not installed.
#ifdef GR_HAVE_UHD

#define GR_UHD_API

//suppress 319. No access specifier given for base class name (ignored).
#pragma SWIG nowarn=319

////////////////////////////////////////////////////////////////////////
// standard includes
////////////////////////////////////////////////////////////////////////
%include "gnuradio.i"

//load generated python docstrings
%include "uhd_swig_doc.i"

////////////////////////////////////////////////////////////////////////
// block headers
////////////////////////////////////////////////////////////////////////
%{
#include <gr_uhd_usrp_source.h>
#include <gr_uhd_usrp_sink.h>
#include <gr_uhd_amsg_source.h>
%}

////////////////////////////////////////////////////////////////////////
// used types
////////////////////////////////////////////////////////////////////////
%template(string_vector_t) std::vector<std::string>;

%template(size_vector_t) std::vector<size_t>;

%include <uhd/config.hpp>

%include <uhd/utils/pimpl.hpp>

%ignore uhd::dict::operator[]; //ignore warnings about %extend
%include <uhd/types/dict.hpp>
%template(string_string_dict_t) uhd::dict<std::string, std::string>; //define after dict

%include <uhd/types/device_addr.hpp>

%include <uhd/types/io_type.hpp>

%template(range_vector_t) std::vector<uhd::range_t>; //define before range
%include <uhd/types/ranges.hpp>

%include <uhd/types/tune_request.hpp>

%include <uhd/types/tune_result.hpp>

%include <uhd/types/io_type.hpp>

%include <uhd/types/time_spec.hpp>

%include <uhd/types/clock_config.hpp>

%include <uhd/types/metadata.hpp>

%template(device_addr_vector_t) std::vector<uhd::device_addr_t>;

%include <uhd/types/sensors.hpp>

////////////////////////////////////////////////////////////////////////
// swig dboard_iface for python access
////////////////////////////////////////////////////////////////////////
%include stdint.i
%include <uhd/types/serial.hpp>
%template(byte_vector_t) std::vector<uint8_t>;
%include <uhd/usrp/dboard_iface.hpp>

%template(dboard_iface_sptr) boost::shared_ptr<uhd::usrp::dboard_iface>;

////////////////////////////////////////////////////////////////////////
// block magic
////////////////////////////////////////////////////////////////////////
GR_SWIG_BLOCK_MAGIC(uhd,usrp_source)
%include <gr_uhd_usrp_source.h>

GR_SWIG_BLOCK_MAGIC(uhd,usrp_sink)
%include <gr_uhd_usrp_sink.h>

GR_SWIG_BLOCK_MAGIC(uhd,amsg_source)
%include <gr_uhd_amsg_source.h>

////////////////////////////////////////////////////////////////////////
// device discovery (no need to %include device.hpp)
////////////////////////////////////////////////////////////////////////
%{
static uhd::device_addrs_t find_devices_raw(const uhd::device_addr_t &dev_addr = uhd::device_addr_t()){
    return uhd::device::find(dev_addr);
}
%}

static uhd::device_addrs_t find_devices_raw(const uhd::device_addr_t &dev_addr = uhd::device_addr_t());

////////////////////////////////////////////////////////////////////////
// helpful constants
////////////////////////////////////////////////////////////////////////
%{
static const size_t ALL_MBOARDS = uhd::usrp::multi_usrp::ALL_MBOARDS;
%}
static const size_t ALL_MBOARDS;

#if SWIGGUILE
%scheme %{
(load-extension-global "libguile-gnuradio-uhd_swig" "scm_init_gnuradio_uhd_swig_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif  /* SWIGGUILE */

#endif  /* GR_HAVE_UHD */
