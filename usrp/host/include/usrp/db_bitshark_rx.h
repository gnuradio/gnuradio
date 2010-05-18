/* -*- c++ -*- */
//
// Copyright 2010 Free Software Foundation, Inc.
// 
// This file is part of GNU Radio
// 
// GNU Radio is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either asversion 3, or (at your option)
// any later version.
// 
// GNU Radio is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with GNU Radio; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street,
// Boston, MA 02110-1301, USA.

#ifndef DB_BITSHARK_RX_H
#define DB_BITSHARK_RX_H

#include <usrp/db_base.h>
#include <vector>
#include <stdint.h>

class db_bitshark_rx : public db_base
{
private:
    int d_i2c_addr;
    // Internal function for interfacing to the card
    void _set_pga(int pga_gain);
    
protected:
    void shutdown();
    
public:
    db_bitshark_rx(usrp_basic_sptr usrp, int which);
    ~db_bitshark_rx();
    
    float gain_min();
    float gain_max();
    float gain_db_per_step();
    double freq_min();
    double freq_max();
    struct freq_result_t set_freq(double freq);
    bool  set_gain(float gain);
    bool  set_bw(float bw);
    bool  set_clock_scheme(uint8_t clock_scheme, uint32_t ref_clock_freq);
    bool  is_quadrature();
    bool  i_and_q_swapped();
};

#endif
