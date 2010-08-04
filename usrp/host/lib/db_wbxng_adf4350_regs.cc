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

#include "db_wbxng_adf4350_regs.h"
#include "db_wbxng_adf4350.h"

//#include "cal_div.h"

/* reg 0 */
/* reg 1 */
const uint16_t adf4350_regs::s_phase = 0;
/* reg 2 */
const uint8_t adf4350_regs::s_low_noise_and_low_spur_modes = 3;
const uint8_t adf4350_regs::s_muxout = 6;
const uint8_t adf4350_regs::s_reference_doubler = 0;
const uint8_t adf4350_regs::s_rdiv2 = 0;
const uint8_t adf4350_regs::s_double_buff = 0;
const uint8_t adf4350_regs::s_charge_pump_setting = 5;
const uint8_t adf4350_regs::s_ldf = 0;
const uint8_t adf4350_regs::s_ldp = 0;
const uint8_t adf4350_regs::s_pd_polarity = 1;
const uint8_t adf4350_regs::s_power_down = 0;
const uint8_t adf4350_regs::s_cp_three_state = 0;
const uint8_t adf4350_regs::s_counter_reset = 0;
/* reg 3 */
const uint8_t adf4350_regs::s_csr = 0;
const uint8_t adf4350_regs::s_clk_div_mode = 0;
const uint16_t adf4350_regs::s_12_bit_clock_divider_value = 0;
/* reg 4 */
const uint8_t adf4350_regs::s_feedback_select = 1;
const uint8_t adf4350_regs::s_vco_power_down = 0;
const uint8_t adf4350_regs::s_mtld = 0;
const uint8_t adf4350_regs::s_aux_output_select = 1;
const uint8_t adf4350_regs::s_aux_output_enable = 0;
const uint8_t adf4350_regs::s_aux_output_power = 0;
const uint8_t adf4350_regs::s_rf_output_enable = 1;
const uint8_t adf4350_regs::s_output_power = 3;
/* reg 5 */
const uint8_t adf4350_regs::s_ld_pin_mode = 1;

adf4350_regs::adf4350_regs(){

    /* reg 0 */
    d_int = uint16_t(100);
    d_frac = 0;
    /* reg 1 */
    d_prescaler = uint8_t(0);
    d_mod = uint16_t(0xfff);                      /* max fractional accuracy */
    /* reg 2 */
    d_10_bit_r_counter = uint16_t(2);
    /* reg 3 */
    /* reg 4 */
    d_divider_select = 0;
    d_8_bit_band_select_clock_divider_value = 0;
    /* reg 5 */
}

adf4350_regs::~adf4350_regs(void){
}

uint32_t
adf4350_regs::_reg_shift(uint32_t data, uint32_t shift){
        return data << shift;
}

uint32_t
adf4350_regs::compute_register(uint8_t addr){
	uint32_t data = 0;
	switch (addr){
		case 0: data = (
			_reg_shift(d_int, 15)                           |
			_reg_shift(d_frac, 3));
		        break;
		case 1: data = (
			_reg_shift(d_prescaler, 27)                     |
			_reg_shift(s_phase, 15)                         |
			_reg_shift(d_mod, 3));
		        break;
		case 2: data = (
			_reg_shift(s_low_noise_and_low_spur_modes, 29)  |
			_reg_shift(s_muxout, 26)                        |
			_reg_shift(s_reference_doubler, 25)             |
			_reg_shift(s_rdiv2, 24)                         |
			_reg_shift(d_10_bit_r_counter, 14)              |
			_reg_shift(s_double_buff, 13)                   |
			_reg_shift(s_charge_pump_setting, 9)            |
			_reg_shift(s_ldf, 8)                            |
			_reg_shift(s_ldp, 7)                            |
			_reg_shift(s_pd_polarity, 6)                    |
			_reg_shift(s_power_down, 5)                     |
			_reg_shift(s_cp_three_state, 4)                 |
			_reg_shift(s_counter_reset, 3));
		        break;
		case 3: data = (
			_reg_shift(s_csr, 18)                           |
			_reg_shift(s_clk_div_mode, 15)                  |
			_reg_shift(s_12_bit_clock_divider_value, 3));
		        break;
		case 4: data = (
			_reg_shift(s_feedback_select, 23)               |
			_reg_shift(d_divider_select, 20)                |
			_reg_shift(d_8_bit_band_select_clock_divider_value, 12) |
			_reg_shift(s_vco_power_down, 11)                |
			_reg_shift(s_mtld, 10)                          |
			_reg_shift(s_aux_output_select, 9)              |
			_reg_shift(s_aux_output_enable, 8)              |
			_reg_shift(s_aux_output_power, 6)               |
			_reg_shift(s_rf_output_enable, 5)               |
			_reg_shift(s_output_power, 3));
		        break;
		case 5: data = (
			_reg_shift(s_ld_pin_mode, 22));
		        break;
		default: return data;
	}
	/* return the data to write out to spi */
    return data;
}

