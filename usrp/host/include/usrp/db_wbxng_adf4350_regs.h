/*
 * Copyright 2009 Ettus Research LLC
 */

#ifndef ADF4350_REGS_H
#define ADF4350_REGS_H

#include <usrp/db_wbxng_adf4350.h>
#include <usrp/db_base.h>
#include <stdint.h>

class adf4350;

class adf4350_regs
{
public:
    adf4350_regs(adf4350* _adf4350);
    ~adf4350_regs();

    adf4350* d_adf4350;

    uint32_t _reg_shift(uint32_t data, uint32_t shift);
    void _load_register(uint8_t addr);

    /* reg 0 */
    uint16_t _int;
    uint16_t _frac;
    /* reg 1 */
    static uint8_t _prescaler;
    static uint16_t _phase;
    uint16_t _mod;
    /* reg 2 */
    static uint8_t _low_noise_and_low_spur_modes;
    static uint8_t _muxout;
    static uint8_t _reference_doubler;
    static uint8_t _rdiv2;
    uint16_t _10_bit_r_counter;
    static uint8_t _double_buff;
    static uint8_t _charge_pump_setting;
    static uint8_t _ldf;
    static uint8_t _ldp;
    static uint8_t _pd_polarity;
    static uint8_t _power_down;
    static uint8_t _cp_three_state;
    static uint8_t _counter_reset;
    /* reg 3 */
    static uint8_t _csr;
    static uint8_t _clk_div_mode;
    static uint16_t _12_bit_clock_divider_value;
    /* reg 4 */
    static uint8_t _feedback_select;
    uint8_t _divider_select;
    uint8_t _8_bit_band_select_clock_divider_value;
    static uint8_t _vco_power_down;
    static uint8_t _mtld;
    static uint8_t _aux_output_select;
    static uint8_t _aux_output_enable;
    static uint8_t _aux_output_power;
    static uint8_t _rf_output_enable;
    static uint8_t _output_power;
    /* reg 5 */
    static uint8_t _ld_pin_mode;
};

#endif /* ADF4350_REGS_H */
