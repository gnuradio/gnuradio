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


#include <db_base.h>

bool db_basic_init(struct db_base *db);
bool db_basic_set_freq(struct db_base *db, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc);
bool db_basic_set_gain(struct db_base *db, u2_fxpt_gain_t gain);
bool db_basic_set_tx_enable(struct db_base *, bool on);

struct db_basic {
  struct db_base	base;
};


struct db_basic db_basic_tx = {
  .base.dbid = 0x0000,
  .base.is_tx = true,
  .base.output_enables = 0x0000,
  .base.used_pins = 0x0000,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(-90e9),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(90e9),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.atr_mask = 	0,
  .base.atr_txval = 	0,
  .base.atr_rxval =	0,
  //.base.atr_tx_delay =	0,
  //.base.atr_rx_delay =	0,

  .base.init = db_basic_init,
  .base.set_freq = db_basic_set_freq,
  .base.set_gain = db_basic_set_gain,
  .base.set_tx_enable = db_basic_set_tx_enable,
  .base.set_antenna = 0,
};

struct db_basic db_basic_rx = {
  .base.dbid = 0x0001,
  .base.is_tx = false,
  .base.output_enables = 0x0000,
  .base.used_pins = 0x0000,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(-90e9),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(90e9),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.is_quadrature = false,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.atr_mask = 	0,
  .base.atr_txval = 	0,
  .base.atr_rxval =	0,
  //.base.atr_tx_delay =	0,
  //.base.atr_rx_delay =	0,

  .base.init = db_basic_init,
  .base.set_freq = db_basic_set_freq,
  .base.set_gain = db_basic_set_gain,
  .base.set_tx_enable = db_basic_set_tx_enable,
  .base.set_antenna = 0,
};

struct db_basic db_lf_tx = {
  .base.dbid = 0x000e,
  .base.is_tx = true,
  .base.output_enables = 0x0000,
  .base.used_pins = 0x0000,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(-32e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(32e6),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.atr_mask = 	0,
  .base.atr_txval = 	0,
  .base.atr_rxval =	0,
  //.base.atr_tx_delay =	0,
  //.base.atr_rx_delay =	0,

  .base.init = db_basic_init,
  .base.set_freq = db_basic_set_freq,
  .base.set_gain = db_basic_set_gain,
  .base.set_tx_enable = db_basic_set_tx_enable,
  .base.set_antenna = 0,
};

struct db_basic db_lf_rx = {
  .base.dbid = 0x000f,
  .base.is_tx = false,
  .base.output_enables = 0x0000,
  .base.used_pins = 0x0000,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(32e6),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.is_quadrature = false,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.atr_mask = 	0,
  .base.atr_txval = 	0,
  .base.atr_rxval =	0,
  //.base.atr_tx_delay =	0,
  //.base.atr_rx_delay =	0,

  .base.init = db_basic_init,
  .base.set_freq = db_basic_set_freq,
  .base.set_gain = db_basic_set_gain,
  .base.set_tx_enable = db_basic_set_tx_enable,
  .base.set_antenna = 0,
};


bool
db_basic_init(struct db_base *db)
{
  return true;
}

bool
db_basic_set_freq(struct db_base *db, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc)
{
  *dc = 0;
  return true;
}

bool
db_basic_set_gain(struct db_base *db, u2_fxpt_gain_t gain)
{
  return true;
}

bool
db_basic_set_tx_enable(struct db_base *db, bool on)
{
  return true;
}

