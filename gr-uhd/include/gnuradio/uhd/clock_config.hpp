//
// Copyright 2010-2011 Ettus Research LLC
// Copyright 2018 Ettus Research, a National Instruments Company
// Copyright 2019 Ettus Research, a National Instruments Company
// Copyright 2019 Free Software Foundation, Inc.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INCLUDED_UHD_TYPES_CLOCK_CONFIG_HPP
#define INCLUDED_UHD_TYPES_CLOCK_CONFIG_HPP

#include <uhd/config.hpp>

namespace uhd {

/*!  The DEPRECATED Clock configuration settings:
 *
 * This is a copy from the UHD, for backward compatibility. UHD has deprecated
 * this structure in UHD 4.0. Older versions of UHD still have it, but it won't
 * be modified any more.
 */
struct clock_config_t {
    //------ simple usage --------//
    clock_config_t(void) : ref_source(REF_INT), pps_source(PPS_SMA), pps_polarity(PPS_POS)
    {
        /* NOP */
    }

    //! A convenience function to create an external clock configuration
    static clock_config_t external(void)
    {
        clock_config_t clock_config;
        clock_config.ref_source = clock_config_t::REF_SMA;
        clock_config.pps_source = clock_config_t::PPS_SMA;
        clock_config.pps_polarity = clock_config_t::PPS_POS;
        return clock_config;
    }

    //! A convenience function to create an internal clock configuration
    static clock_config_t internal(void)
    {
        clock_config_t clock_config;
        clock_config.ref_source = clock_config_t::REF_INT;
        clock_config.pps_source = clock_config_t::PPS_SMA;
        clock_config.pps_polarity = clock_config_t::PPS_POS;
        return clock_config;
    }

    //------ advanced usage --------//
    enum ref_source_t {
        REF_AUTO = int('a'), // automatic (device specific)
        REF_INT = int('i'),  // internal reference
        REF_SMA = int('s'),  // external sma port
        REF_MIMO = int('m')  // reference from mimo cable
    } ref_source;
    enum pps_source_t {
        PPS_INT = int('i'), // there is no internal
        PPS_SMA = int('s'), // external sma port
        PPS_MIMO = int('m') // time sync from mimo cable
    } pps_source;
    enum pps_polarity_t {
        PPS_NEG = int('n'), // negative edge
        PPS_POS = int('p')  // positive edge
    } pps_polarity;
};

} // namespace uhd

#endif /* INCLUDED_UHD_TYPES_CLOCK_CONFIG_HPP */
