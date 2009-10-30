/*
 * Copyright 2009 Ettus Research LLC
 */

#ifndef ADF4350_REGS_H
#define ADF4350_REGS_H

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
};

#endif /* ADF4350_REGS_H */
