/* -*- c -*- */
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_USRP2_CLOCK_CONFIG_H
#define INCLUDED_USRP2_CLOCK_CONFIG_H

// FIXME: This duplicates the firmware usrp2_mimo_config.h file

namespace usrp2 {

    struct clock_config_t{
        //10 mhz reference source (internal, sma, or mimo)
        enum{
            REF_INT,
            REF_SMA,
            REF_MIMO
        } ref_source;

        //10 mhz reference option
        bool provide_ref_to_mimo;

        //pps source (sma, or mimo)
        enum{
            PPS_SMA,
            PPS_MIMO
        } pps_source;

        //pps polarity (negedge, or posedge)
        enum{
            PPS_NEG,
            PPS_POS
        } pps_polarity;

        clock_config_t(void){
            ref_source = REF_INT;
            provide_ref_to_mimo = false;
            pps_source = PPS_SMA;
            pps_polarity = PPS_NEG;
        }

    };

}

#endif /* INCLUDED_USRP2_CLOCK_CONFIG_H */
