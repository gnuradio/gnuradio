//
// Copyright 2009 Free Software Foundation, Inc.
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

#ifndef ADF4350_REGS_H
#define ADF4350_REGS_H

#include <usrp/db_base.h>
#include <stdint.h>

class adf4350;

class adf4350_regs
{
public:
    adf4350_regs();
    ~adf4350_regs();

    uint32_t _reg_shift(uint32_t data, uint32_t shift);
    uint32_t compute_register(uint8_t addr);

    /* reg 0 */
    uint16_t d_int;
    uint16_t d_frac;
    /* reg 1 */
    uint8_t d_prescaler;
    static const uint16_t s_phase;
    uint16_t d_mod;
    /* reg 2 */
    static const uint8_t s_low_noise_and_low_spur_modes;
    static const uint8_t s_muxout;
    static const uint8_t s_reference_doubler;
    static const uint8_t s_rdiv2;
    uint16_t d_10_bit_r_counter;
    static const uint8_t s_double_buff;
    static const uint8_t s_charge_pump_setting;
    static const uint8_t s_ldf;
    static const uint8_t s_ldp;
    static const uint8_t s_pd_polarity;
    static const uint8_t s_power_down;
    static const uint8_t s_cp_three_state;
    static const uint8_t s_counter_reset;
    /* reg 3 */
    static const uint8_t s_csr;
    static const uint8_t s_clk_div_mode;
    static const uint16_t s_12_bit_clock_divider_value;
    /* reg 4 */
    static const uint8_t s_feedback_select;
    uint8_t d_divider_select;
    uint8_t d_8_bit_band_select_clock_divider_value;
    static const uint8_t s_vco_power_down;
    static const uint8_t s_mtld;
    static const uint8_t s_aux_output_select;
    static const uint8_t s_aux_output_enable;
    static const uint8_t s_aux_output_power;
    static const uint8_t s_rf_output_enable;
    static const uint8_t s_output_power;
    /* reg 5 */
    static const uint8_t s_ld_pin_mode;

protected:
    usrp_basic_sptr d_usrp;
    int d_spi_enable;
    int d_spi_format;
};

#endif /* ADF4350_REGS_H */
