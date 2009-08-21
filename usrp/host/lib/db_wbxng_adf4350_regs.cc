/*
 * Copyright 2009 Ettus Research LLC
 */

#include <usrp/db_wbxng_adf4350_regs.h>
#include <usrp/db_wbxng_adf4350.h>
//#include "cal_div.h"

adf4350_regs::adf4350_regs(adf4350* _adf4350){
    d_adf4350 = _adf4350;

    /* reg 0 */
    _int = uint16_t(100);
    _frac = 0;
    /* reg 1 */
    _prescaler = 1;                        /* 8/9 */
    _phase = 0;                           /* 0 */
    _mod = uint16_t(0xfff);                      /* max fractional accuracy */
    /* reg 2 */
    _low_noise_and_low_spur_modes = 0;     /* low noise mode */
    _muxout = 6;                           /* digital lock detect */
    _reference_doubler = 1;                /* enabled */
    _rdiv2 = 0;                            /* disabled */
    _10_bit_r_counter = uint16_t(1);
    _double_buff = 0;                      /* disabled */
    _charge_pump_setting = 7;              /* 2.50 mA */
    _ldf = 0;                              /* frac-n */
    _ldp = 0;                              /* 10 ns */
    _pd_polarity = 1;                      /* positive */
    _power_down = 0;                       /* disabled */
    _cp_three_state = 0;                   /* disabled */
    _counter_reset = 0;                    /* disabled */
    /* reg 3 */
    _csr = 0;                              /* disabled */
    _clk_div_mode = 0;                     /* clock divider off */
    _12_bit_clock_divider_value = 0;      /* 0 */
    /* reg 4 */
    _feedback_select = 1;                  /* fundamental */
    _divider_select = 0;
    _8_bit_band_select_clock_divider_value = 0;
    _vco_power_down = 0;                   /* vco powered up */
    _mtld = 0;                             /* mute disabled */
    _aux_output_select = 0;                /* divided output */
    _aux_output_enable = 0;                /* disabled */
    _aux_output_power = 0;                 /* -4 */
    _rf_output_enable = 1;                 /* enabled */
    _output_power = 1;                     /* -1 */
    /* reg 5 */
    _ld_pin_mode = 1;                      /* digital lock detect */
}

adf4350_regs::~adf4350_regs(void){
}

uint32_t 
adf4350_regs::_reg_shift(uint32_t data, uint32_t shift){
        return data << shift;
    }

void 
adf4350_regs::_load_register(uint8_t addr){
	uint32_t data;
	switch (addr){
		case 0: data = (
			_reg_shift(_int, 15)                           |
			_reg_shift(_frac, 3)); break;
		case 1: data = (
			_reg_shift(_prescaler, 27)                     |
			_reg_shift(_phase, 15)                         |
			_reg_shift(_mod, 3)); break;
		case 2: data = (
			_reg_shift(_low_noise_and_low_spur_modes, 29)  |
			_reg_shift(_muxout, 26)                        |
			_reg_shift(_reference_doubler, 25)             |
			_reg_shift(_rdiv2, 24)                         |
			_reg_shift(_10_bit_r_counter, 14)              |
			_reg_shift(_double_buff, 13)                   |
			_reg_shift(_charge_pump_setting, 9)            |
			_reg_shift(_ldf, 8)                            |
			_reg_shift(_ldp, 7)                            |
			_reg_shift(_pd_polarity, 6)                    |
			_reg_shift(_power_down, 5)                     |
			_reg_shift(_cp_three_state, 4)                 |
			_reg_shift(_counter_reset, 3)); break;
		case 3: data = (
			_reg_shift(_csr, 18)                           |
			_reg_shift(_clk_div_mode, 15)                  |
			_reg_shift(_12_bit_clock_divider_value, 3)); break;
		case 4: data = (
			_reg_shift(_feedback_select, 23)               |
			_reg_shift(_divider_select, 20)                |
			_reg_shift(_8_bit_band_select_clock_divider_value, 12) |
			_reg_shift(_vco_power_down, 11)                |
			_reg_shift(_mtld, 10)                          |
			_reg_shift(_aux_output_select, 9)              |
			_reg_shift(_aux_output_enable, 8)              |
			_reg_shift(_aux_output_power, 6)               |
			_reg_shift(_rf_output_enable, 5)               |
			_reg_shift(_output_power, 3)); break;
		case 5: data = (
			_reg_shift(_ld_pin_mode, 22)); break;
		default: return;
	}
	/* write the data out to spi */
	d_adf4350->_write(addr, data);
}
