/* -*- c++ -*- */
/*
 * Copyright 2010-2014 Free Software Foundation, Inc.
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

%include <std_vector.i>
%include "gnuradio.i"

//load generated python docstrings
%include "uhd_swig_doc.i"

////////////////////////////////////////////////////////////////////////
// SWIG should not see the uhd::usrp::multi_usrp class
////////////////////////////////////////////////////////////////////////
%ignore gr::uhd::usrp_sink::get_device;
%ignore gr::uhd::usrp_source::get_device;

////////////////////////////////////////////////////////////////////////
// block headers
////////////////////////////////////////////////////////////////////////
%{
#include <gnuradio/uhd/usrp_source.h>
#include <gnuradio/uhd/usrp_sink.h>
#include <gnuradio/uhd/amsg_source.h>
%}

%include "gnuradio/uhd/usrp_block.h"

////////////////////////////////////////////////////////////////////////
// used types
////////////////////////////////////////////////////////////////////////

%template(uhd_string_vector_t) std::vector<std::string>;

%template(uhd_size_vector_t) std::vector<size_t>;

%include <uhd/config.hpp>

%include <uhd/utils/pimpl.hpp>

%ignore uhd::dict::operator[]; //ignore warnings about %extend
%include <uhd/types/dict.hpp>
%template(string_string_dict_t) uhd::dict<std::string, std::string>; //define after dict

%extend uhd::dict<std::string, std::string>{
    std::string __getitem__(std::string key) {return (*self)[key];}
    void __setitem__(std::string key, std::string val) {(*self)[key] = val;}
};

%include <uhd/types/device_addr.hpp>

%include <uhd/types/io_type.hpp>

%template(range_vector_t) std::vector<uhd::range_t>; //define before range
%include <uhd/types/ranges.hpp>

%include <uhd/types/tune_request.hpp>

%include <uhd/types/tune_result.hpp>

%include <uhd/types/io_type.hpp>

%include <uhd/types/time_spec.hpp>

%extend uhd::time_spec_t{
    uhd::time_spec_t __add__(const uhd::time_spec_t &what)
    {
        uhd::time_spec_t temp = *self;
        temp += what;
        return temp;
    }
    uhd::time_spec_t __sub__(const uhd::time_spec_t &what)
    {
        uhd::time_spec_t temp = *self;
        temp -= what;
        return temp;
    }
    bool __eq__(const uhd::time_spec_t &what)
    {
      return (what == *self);
    }
};

%include <uhd/types/stream_cmd.hpp>

%include <uhd/types/clock_config.hpp>

%include <uhd/types/metadata.hpp>

%template(device_addr_vector_t) std::vector<uhd::device_addr_t>;

%include <uhd/types/sensors.hpp>

%include <uhd/stream.hpp>

////////////////////////////////////////////////////////////////////////
// swig dboard_iface for python access
////////////////////////////////////////////////////////////////////////
%include stdint.i
%include <uhd/types/serial.hpp>
%include <uhd/usrp/dboard_iface.hpp>

%template(dboard_iface_sptr) boost::shared_ptr<uhd::usrp::dboard_iface>;

////////////////////////////////////////////////////////////////////////
// block magic
////////////////////////////////////////////////////////////////////////
%include <gnuradio/uhd/usrp_source.h>
%include <gnuradio/uhd/usrp_sink.h>
%include <gnuradio/uhd/amsg_source.h>

GR_SWIG_BLOCK_MAGIC2(uhd, usrp_source)
GR_SWIG_BLOCK_MAGIC2(uhd, usrp_sink)
GR_SWIG_BLOCK_MAGIC2(uhd, amsg_source)

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
static const size_t ALL_CHANS = uhd::usrp::multi_usrp::ALL_CHANS;
static const std::string ALL_GAINS = uhd::usrp::multi_usrp::ALL_GAINS;

#ifdef UHD_USRP_MULTI_USRP_LO_CONFIG_API
static const std::string ALL_LOS = uhd::usrp::multi_usrp::ALL_LOS;
#else
static const std::string ALL_LOS;
#endif
%}

static const size_t ALL_MBOARDS;
static const size_t ALL_CHANS;
static const std::string ALL_GAINS;
static const std::string ALL_LOS;

%{
#include <uhd/version.hpp>
std::string get_version_string(void){
    return uhd::get_version_string();
}
%}
std::string get_version_string(void);

#endif  /* GR_HAVE_UHD */
