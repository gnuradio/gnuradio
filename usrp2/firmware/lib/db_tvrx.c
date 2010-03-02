/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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
 */

#include <i2c.h>
#include <lsdac.h>
#include <memory_map.h>
#include <db_base.h>
#include <hal_io.h>
#include <stdio.h>

bool tvrx_init(struct db_base *db);
bool tvrx_set_freq(struct db_base *db, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc);
bool tvrx_set_gain(struct db_base *db, u2_fxpt_gain_t gain);

#define I2C_ADDR 0x60
#define REF_FREQ (U2_DOUBLE_TO_FXPT_FREQ(4e6)/640*8)

#define ref_div 640  /* choices are 640, 512, 1024 */

#if (ref_div == 640)
#define ref_div_byte 0
#else
#if (ref_div == 512)
#define ref_div_byte 0x6
#else
#define ref_div_byte 0x2
#endif
#endif

#define fast_tuning 0x40

#define control_byte_1 (0x88|fast_tuning|ref_div_byte)


struct db_tvrx_common {
  // TVRX common stuff
  u2_fxpt_freq_t first_if;
  u2_fxpt_freq_t second_if;
};

struct db_tvrx_dummy {
  struct db_base	base;
  struct db_tvrx_common	common;
};

struct db_tvrx1 {
  struct db_base	base;
  struct db_tvrx_common	common;
};

struct db_tvrx2 {
  struct db_base	base;
  struct db_tvrx_common	common;
};

struct db_tvrx3 {
  struct db_base	base;
  struct db_tvrx_common	common;
};

/* The class instances */
struct db_tvrx1 db_tvrx1 = {
  .base.dbid = 0x0003,
  .base.is_tx = false,
  .base.output_enables = 0x0000,
  .base.used_pins = 0x0000,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(50e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(860e6),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(95),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(1),
  .base.is_quadrature = false,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.init = tvrx_init,
  .base.set_freq = tvrx_set_freq,
  .base.set_gain = tvrx_set_gain,
  .base.set_tx_enable = 0,
  .base.atr_mask = 0x0000,
  .base.atr_txval = 0,
  .base.atr_rxval = 0,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.first_if = U2_DOUBLE_TO_FXPT_FREQ(43.75e6),
  .common.second_if = U2_DOUBLE_TO_FXPT_FREQ(5.75e6),
};

#if 0
struct db_tvrx2 db_tvrx2 = {
  .base.dbid = 0x000c,
  .base.is_tx = false,
  .base.output_enables = 0x0000,
  .base.used_pins = 0x0000,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(50e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(860e6),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(95),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(1),
  .base.is_quadrature = false,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.init = tvrx_init,
  .base.set_freq = tvrx_set_freq,
  .base.set_gain = tvrx_set_gain,
  .base.set_tx_enable = 0,
  .base.atr_mask = 0x0000,
  .base.atr_txval = 0,
  .base.atr_rxval = 0,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.first_if = U2_DOUBLE_TO_FXPT_FREQ(44e6),
  .common.second_if = U2_DOUBLE_TO_FXPT_FREQ(56e6),	// Fs - 44e6
};
#endif

struct db_tvrx3 db_tvrx3 = {
  .base.dbid = 0x0040,
  .base.is_tx = false,
  .base.output_enables = 0x0000,
  .base.used_pins = 0x0000,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(50e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(860e6),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(95),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(1),
  .base.is_quadrature = false,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.init = tvrx_init,
  .base.set_freq = tvrx_set_freq,
  .base.set_gain = tvrx_set_gain,
  .base.set_tx_enable = 0,
  .base.atr_mask = 0x0000,
  .base.atr_txval = 0,
  .base.atr_rxval = 0,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.first_if = U2_DOUBLE_TO_FXPT_FREQ(44e6),
  .common.second_if = U2_DOUBLE_TO_FXPT_FREQ(56e6),	// Fs - 44e6
};

bool
tvrx_init(struct db_base *dbb)
{
  struct db_tvrx_dummy *db = (struct db_tvrx_dummy *) dbb;
  db->base.set_gain(dbb,U2_DOUBLE_TO_FXPT_GAIN(94.0));
  return true;
}

bool
tvrx_set_freq(struct db_base *dbb, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc)
{
  *dc = 0;
  if (freq < dbb->freq_min || freq > dbb->freq_max)
    return false;

  struct db_tvrx_dummy *db = (struct db_tvrx_dummy *) dbb;

  u2_fxpt_freq_t target_lo_freq = freq + db->common.first_if;
  int n_div = u2_fxpt_freq_round_to_int(((1LL<<20) * target_lo_freq)/REF_FREQ);
  
  u2_fxpt_freq_t actual_lo_freq = REF_FREQ * n_div;
  u2_fxpt_freq_t actual_freq = actual_lo_freq - db->common.first_if;
  if(n_div > 32767)
    return false;

  if (0)
    printf("n_div = %d, actual_freq = %d, actual_lo_freq = %d\n",
	   n_div, u2_fxpt_freq_round_to_int(actual_freq),
	   u2_fxpt_freq_round_to_int(actual_lo_freq));

  unsigned char buf[4];
  buf[0] = (n_div>>8) & 0xff;
  buf[1] = n_div & 0xff;
  buf[2] = control_byte_1;
  buf[3] = ((actual_freq < U2_DOUBLE_TO_FXPT_FREQ(158e6)) ? 0xa8 :  // VHF LOW
	    (actual_freq < U2_DOUBLE_TO_FXPT_FREQ(464e6)) ? 0x98 :  // VHF HIGH
	    0x38);  // UHF

  *dc = actual_freq - db->common.second_if;
  return i2c_write(I2C_ADDR,buf,4);
}

bool
tvrx_set_gain(struct db_base *dbb, u2_fxpt_gain_t gain)
{
  //struct db_tvrx_dummy *db = (struct db_tvrx_dummy *) dbb;
  int rfgain;
  int ifgain;
  if(gain>U2_DOUBLE_TO_FXPT_GAIN(95.0))
    return false;
  if(gain<0)
    return false;

  if(gain>U2_DOUBLE_TO_FXPT_GAIN(60.0)) {
    rfgain = U2_DOUBLE_TO_FXPT_GAIN(60.0);
    ifgain = gain-U2_DOUBLE_TO_FXPT_GAIN(60.0);
  } else {
    rfgain = gain;
    ifgain = 0;
  }
  
  int rf_slope_q8 = 256 * 4096 * 2.5 / 60.0 / 1.22 / 3.3;
  int rf_offset_q8 = 128 * 256 * 4096 * 1.25 / 1.22 / 3.3;
  int if_slope_q8 = 256 * 4096 * 2.25 / 35.0 / 1.22 / 3.3;
  int if_offset_q8 = 128 * 256 * 4096 * 1.4 / 1.22 / 3.3;

  
  int rfdac = (rfgain*rf_slope_q8 + rf_offset_q8)>>15;
  int ifdac = (ifgain*if_slope_q8 + if_offset_q8)>>15;
  lsdac_write_rx(0,rfdac);
  lsdac_write_rx(1,ifdac);

  if (0)
    printf("Setting gain %d, rf %d, if %d\n",gain,rfdac,ifdac);

  return true;
}


bool
tvrx_lock_detect(struct db_base *dbb)
{
  // struct db_tvrx_dummy *db = (struct db_tvrx_dummy *) dbb;
  return true;
}
