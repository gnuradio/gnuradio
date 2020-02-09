/* -*- c++ -*- */
/*
 * Copyright 2010-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

// Defined during configure; avoids trying to locate
// header files if UHD was not installed.
#ifdef GR_HAVE_UHD

#define GR_UHD_API

#include <uhd/version.hpp>

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

%template(range_vector_t) std::vector<uhd::range_t>; //define before range
%include <uhd/types/ranges.hpp>

%include <uhd/types/tune_request.hpp>

%include <uhd/types/tune_result.hpp>

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

%include <uhd/types/metadata.hpp>

%template(device_addr_vector_t) std::vector<uhd::device_addr_t>;

%include <uhd/types/sensors.hpp>

%include <uhd/stream.hpp>

%include <uhd/types/filters.hpp>

%include stdint.i

// Used for lists of filter taps
%template(uhd_vector_int16_t) std::vector<int16_t>;

////////////////////////////////////////////////////////////////////////
// swig dboard_iface for python access
////////////////////////////////////////////////////////////////////////
%include <uhd/types/serial.hpp>
%include <uhd/usrp/dboard_iface.hpp>

#if UHD_VERSION < 4000000 

%template(filter_info_base_sptr) boost::shared_ptr<uhd::filter_info_base>;
%template(analog_filter_base_stpr) boost::shared_ptr<uhd::analog_filter_base>;
%template(analog_filter_lp_stpr) boost::shared_ptr<uhd::analog_filter_lp>;
%template(digital_filter_base_int16_t_sptr) boost::shared_ptr<uhd::digital_filter_base<int16_t>>;
%template(digital_filter_fir_int16_t_sptr) boost::shared_ptr<uhd::digital_filter_fir<int16_t>>;

%extend uhd::filter_info_base{
    boost::shared_ptr<uhd::analog_filter_base> to_analog_info_base(boost::shared_ptr<uhd::filter_info_base> ptr) {
       return boost::dynamic_pointer_cast<uhd::analog_filter_base>(ptr);
    }

    boost::shared_ptr<uhd::analog_filter_lp> to_analog_filter_lp(boost::shared_ptr<uhd::filter_info_base> ptr) {
       return boost::dynamic_pointer_cast<uhd::analog_filter_lp>(ptr);
    }

    boost::shared_ptr<uhd::digital_filter_base<int16_t>> to_digital_filter_base_int16(boost::shared_ptr<uhd::filter_info_base> ptr) {
       return boost::dynamic_pointer_cast<uhd::digital_filter_base<int16_t>>(ptr);
    }

    boost::shared_ptr<uhd::digital_filter_fir<int16_t>> to_digital_filter_fir_int16(boost::shared_ptr<uhd::filter_info_base> ptr) {
       return boost::dynamic_pointer_cast<uhd::digital_filter_fir<int16_t>>(ptr);
    }
}

%template(dboard_iface_sptr) boost::shared_ptr<uhd::usrp::dboard_iface>;

#else

%template(filter_info_base_sptr) std::shared_ptr<uhd::filter_info_base>;
%template(analog_filter_base_stpr) std::shared_ptr<uhd::analog_filter_base>;
%template(analog_filter_lp_stpr) std::shared_ptr<uhd::analog_filter_lp>;
%template(digital_filter_base_int16_t_sptr) std::shared_ptr<uhd::digital_filter_base<int16_t>>;
%template(digital_filter_fir_int16_t_sptr) std::shared_ptr<uhd::digital_filter_fir<int16_t>>;

%extend uhd::filter_info_base{
    std::shared_ptr<uhd::analog_filter_base> to_analog_info_base(std::shared_ptr<uhd::filter_info_base> ptr) {
       return std::dynamic_pointer_cast<uhd::analog_filter_base>(ptr);
    }

    std::shared_ptr<uhd::analog_filter_lp> to_analog_filter_lp(std::shared_ptr<uhd::filter_info_base> ptr) {
       return std::dynamic_pointer_cast<uhd::analog_filter_lp>(ptr);
    }

    std::shared_ptr<uhd::digital_filter_base<int16_t>> to_digital_filter_base_int16(std::shared_ptr<uhd::filter_info_base> ptr) {
       return std::dynamic_pointer_cast<uhd::digital_filter_base<int16_t>>(ptr);
    }

    std::shared_ptr<uhd::digital_filter_fir<int16_t>> to_digital_filter_fir_int16(std::shared_ptr<uhd::filter_info_base> ptr) {
       return std::dynamic_pointer_cast<uhd::digital_filter_fir<int16_t>>(ptr);
    }

%template(dboard_iface_sptr) std::shared_ptr<uhd::usrp::dboard_iface>;

}

#endif

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
