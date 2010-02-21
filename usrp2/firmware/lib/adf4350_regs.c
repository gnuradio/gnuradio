/*
 * Copyright 2010 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "adf4350_regs.h"
#include "adf4350.h"
#include "db_wbxng.h"

#define _REG_SHIFT(reg, shift) (((uint32_t)(reg)) << (shift))

/* reg 0 */
/* reg 1 */
static const uint16_t adf4350_regs_phase = 0;                           /* 0 */
/* reg 2 */
static const uint8_t adf4350_regs_low_noise_and_low_spur_modes = 3;     /* low noise mode */
static const uint8_t adf4350_regs_muxout = 3;                           /* digital lock detect */
static const uint8_t adf4350_regs_reference_doubler = 0;                /* disabled */
static const uint8_t adf4350_regs_rdiv2 = 1;                            /* disabled */
static const uint8_t adf4350_regs_double_buff = 0;                      /* disabled */
static const uint8_t adf4350_regs_charge_pump_setting = 5;              /* 2.50 mA */
static const uint8_t adf4350_regs_ldf = 0;                              /* frac-n */
static const uint8_t adf4350_regs_ldp = 0;                              /* 10 ns */
static const uint8_t adf4350_regs_pd_polarity = 1;                      /* positive */
static const uint8_t adf4350_regs_power_down = 0;                       /* disabled */
static const uint8_t adf4350_regs_cp_three_state = 0;                   /* disabled */
static const uint8_t adf4350_regs_counter_reset = 0;                    /* disabled */
/* reg 3 */
static const uint8_t adf4350_regs_csr = 0;                              /* disabled */
static const uint8_t adf4350_regs_clk_div_mode = 0;                     /* clock divider off */
static const uint16_t adf4350_regs_12_bit_clock_divider_value = 0;      /* 0 */
/* reg 4 */
static const uint8_t adf4350_regs_feedback_select = 1;                  /* fundamental */
static const uint8_t adf4350_regs_vco_power_down = 0;                   /* vco powered up */
static const uint8_t adf4350_regs_mtld = 0;                             /* mute disabled */
static const uint8_t adf4350_regs_aux_output_select = 1;                /* divided output */
static const uint8_t adf4350_regs_aux_output_enable = 1;                /* disabled */
static const uint8_t adf4350_regs_aux_output_power = 0;                 /* -4 */
static const uint8_t adf4350_regs_rf_output_enable = 1;                 /* enabled */
static const uint8_t adf4350_regs_output_power = 3;                     /* -1 */
/* reg 5 */
static const uint8_t adf4350_regs_ld_pin_mode = 1;                      /* digital lock detect */

void adf4350_load_register(uint8_t addr, struct db_base *dbb){
	struct db_wbxng_dummy *db = (struct db_wbxng_dummy *) dbb;
	uint32_t data;
	switch (addr){
		case 0: data = (
			_REG_SHIFT(db->common.adf4350_regs_int, 15)                |
			_REG_SHIFT(db->common.adf4350_regs_frac, 3)); break;
		case 1: data = (
			_REG_SHIFT(db->common.adf4350_regs_prescaler, 27)          |
			_REG_SHIFT(adf4350_regs_phase, 15)                         |
			_REG_SHIFT(db->common.adf4350_regs_mod, 3)); break;
		case 2: data = (
			_REG_SHIFT(adf4350_regs_low_noise_and_low_spur_modes, 29)  |
			_REG_SHIFT(adf4350_regs_muxout, 26)                        |
			_REG_SHIFT(adf4350_regs_reference_doubler, 25)             |
			_REG_SHIFT(adf4350_regs_rdiv2, 24)                         |
			_REG_SHIFT(db->common.adf4350_regs_10_bit_r_counter, 14)   |
			_REG_SHIFT(adf4350_regs_double_buff, 13)                   |
			_REG_SHIFT(adf4350_regs_charge_pump_setting, 9)            |
			_REG_SHIFT(adf4350_regs_ldf, 8)                            |
			_REG_SHIFT(adf4350_regs_ldp, 7)                            |
			_REG_SHIFT(adf4350_regs_pd_polarity, 6)                    |
			_REG_SHIFT(adf4350_regs_power_down, 5)                     |
			_REG_SHIFT(adf4350_regs_cp_three_state, 4)                 |
			_REG_SHIFT(adf4350_regs_counter_reset, 3)); break;
		case 3: data = (
			_REG_SHIFT(adf4350_regs_csr, 18)                           |
			_REG_SHIFT(adf4350_regs_clk_div_mode, 15)                  |
			_REG_SHIFT(adf4350_regs_12_bit_clock_divider_value, 3)); break;
		case 4: data = (
			_REG_SHIFT(adf4350_regs_feedback_select, 23)               |
			_REG_SHIFT(db->common.adf4350_regs_divider_select, 20)     |
			_REG_SHIFT(db->common.adf4350_regs_8_bit_band_select_clock_divider_value, 12) |
			_REG_SHIFT(adf4350_regs_vco_power_down, 11)                |
			_REG_SHIFT(adf4350_regs_mtld, 10)                          |
			_REG_SHIFT(adf4350_regs_aux_output_select, 9)              |
			_REG_SHIFT(adf4350_regs_aux_output_enable, 8)              |
			_REG_SHIFT(adf4350_regs_aux_output_power, 6)               |
			_REG_SHIFT(adf4350_regs_rf_output_enable, 5)               |
			_REG_SHIFT(adf4350_regs_output_power, 3)); break;
		case 5: data = (
			_REG_SHIFT(adf4350_regs_ld_pin_mode, 22)); break;
		default: return;
	}
	/* write the data out to spi */
	adf4350_write(addr, data, dbb);
}
