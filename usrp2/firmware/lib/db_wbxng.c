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


#include "db_wbxng.h"
#include "adf4350.h"
#include <spi.h>
#include <memory_map.h>
#include <db_base.h>
#include <hal_io.h>
#include <mdelay.h>
#include <lsdac.h>
#include <clocks.h>
#include <stdio.h>
#include <stdint.h>

#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#define max(X,Y) ((X) > (Y) ? (X) : (Y))

bool wbxng_init_rx(struct db_base *dbb);
bool wbxng_init_tx(struct db_base *dbb);
bool wbxng_set_freq(struct db_base *dbb, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc);
bool wbxng_set_gain_rx(struct db_base *dbb, u2_fxpt_gain_t gain);
bool wbxng_set_gain_tx(struct db_base *dbb, u2_fxpt_gain_t gain);
bool wbxng_set_tx_enable(struct db_base *dbb, bool on);


/*
 * The class instances
 */
struct db_wbxng_rx db_wbxng_rx = {
  .base.dbid = 0x0053,
  .base.is_tx = false,
  .base.output_enables = RX2_RX1N|RXBB_EN|ATTN_MASK|ENABLE_33|ENABLE_5|PLL_CE|PLL_PDBRF|ATTN_MASK,
  .base.used_pins = 0xFFFF,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(68.75e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(2200e6),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(31.5),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(0.5),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.init = wbxng_init_rx,
  .base.set_freq = wbxng_set_freq,
  .base.set_gain = wbxng_set_gain_rx,
  .base.set_tx_enable = 0,
  .base.atr_mask = RXBB_EN | RX2_RX1N,
  .base.atr_txval = RX2_RX1N,
  .base.atr_rxval = RXBB_EN,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.adf4350_regs_int = UINT16_C(100),
  .common.adf4350_regs_frac = 0,
  .common.adf4350_regs_prescaler = 1,
  .common.adf4350_regs_mod = UINT16_C(0xfff),
  .common.adf4350_regs_10_bit_r_counter = UINT16_C(1),
  .common.adf4350_regs_divider_select = 0,
  .common.adf4350_regs_8_bit_band_select_clock_divider_value = 0,
  .common.spi_mask = SPI_SS_RX_DB,
  .common.freq_mult = 2
};


struct db_wbxng_tx db_wbxng_tx = {
  .base.dbid = 0x0052,
  .base.is_tx = true,
  .base.output_enables = RX_TXN|TXMOD_EN|ENABLE_33|ENABLE_5|PLL_CE|PLL_PDBRF,
  .base.used_pins = 0xFFFF,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(68.75e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(2200e6),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(25),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(0.1),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.init = wbxng_init_tx,
  .base.set_freq = wbxng_set_freq,
  .base.set_gain = wbxng_set_gain_tx,
  .base.set_tx_enable = wbxng_set_tx_enable,
  .base.atr_mask = RX_TXN | TXMOD_EN,
  .base.atr_txval = TXMOD_EN,
  .base.atr_rxval = RX_TXN,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.adf4350_regs_int = UINT16_C(100),
  .common.adf4350_regs_frac = 0,
  .common.adf4350_regs_prescaler = 1,
  .common.adf4350_regs_mod = UINT16_C(0xfff),
  .common.adf4350_regs_10_bit_r_counter = UINT16_C(1),
  .common.adf4350_regs_divider_select = 0,
  .common.adf4350_regs_8_bit_band_select_clock_divider_value = 0,
  .common.spi_mask = SPI_SS_TX_DB,
  .common.freq_mult = 2
};


bool
wbxng_init_tx(struct db_base *dbb)
{
  //struct db_wbxng_dummy *db = (struct db_wbxng_dummy *) dbb;
  clocks_enable_tx_dboard(true, 0);
  hal_gpio_write( GPIO_TX_BANK, ENABLE_5|ENABLE_33, ENABLE_5|ENABLE_33 );

  adf4350_init(dbb);

  // Set the freq now to get the one time 10ms delay out of the way.
  u2_fxpt_freq_t	dc;
  dbb->set_freq(dbb, dbb->freq_min, &dc);
  return true;
}

bool
wbxng_init_rx(struct db_base *dbb)
{
  //struct db_wbxng_dummy *db = (struct db_wbxng_dummy *) dbb;
  clocks_enable_rx_dboard(true, 0);
  hal_gpio_write( GPIO_RX_BANK, ENABLE_5|ENABLE_33, ENABLE_5|ENABLE_33 );

  adf4350_init(dbb);

  // test gain
  dbb->set_gain(dbb,U2_DOUBLE_TO_FXPT_GAIN(20.0));

  // Set the freq now to get the one time 10ms delay out of the way.
  u2_fxpt_freq_t	dc;
  dbb->set_freq(dbb, dbb->freq_min, &dc);

  return true;
}

bool
wbxng_set_freq(struct db_base *dbb, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc)
{
  // clamp freq
  u2_fxpt_freq_t clamp_freq = max(dbb->freq_min, min(freq, dbb->freq_max));
  //printf("Requested LO freq = %u", (uint32_t) ((clamp_freq >> U2_FPF_RP)/1000));
  bool ok = adf4350_set_freq(clamp_freq<<1,dbb);
  *dc = adf4350_get_freq(dbb)>>1;

  return ok;
}

bool
wbxng_set_gain_tx(struct db_base *dbb, u2_fxpt_gain_t gain)
{
  struct db_wbxng_dummy *db = (struct db_wbxng_dummy *) dbb;

  // clamp gain
  //gain = max(db->gain_min, min(gain, db->gain_max));

  int offset_q8 = (int)(1.4/3.3*4096*(1<<15));
  int range_q15 = (int)(-0.9*4096/3.3*256*128);
  int slope_q8 = range_q15/db->base.gain_max;

  int dacword = ((slope_q8 * gain) + offset_q8)>>15;
  //printf("DACWORD 0x%x\n",dacword);
  lsdac_write_tx(0,dacword);
  return true;
}

bool
wbxng_set_gain_rx(struct db_base *dbb, u2_fxpt_gain_t gain)
{
  struct db_wbxng_dummy *db = (struct db_wbxng_dummy *) dbb;

  // clamp gain
  //gain = max(db->gain_min, min(gain, db->gain_max));

  int iobits = (int) ((~((db->base.gain_max - gain) << 2)) & ATTN_MASK);
  //printf("gain %d, gainmax %d, RX_ATTN_MASK = 0x%x, RX_ATTN_WORD = 0x%x\n", gain, db->base.gain_max, (int) (ATTN_MASK), (int) (iobits));

  hal_gpio_write( GPIO_RX_BANK, (int) (iobits), ATTN_MASK );
  return true;
}


bool
wbxng_set_tx_enable(struct db_base *dbb, bool on)
{
  struct db_wbxng_dummy *db = (struct db_wbxng_dummy *) dbb;

  // FIXME

  return false;
}

bool
wbxng_lock_detect(struct db_base *dbb)
{
  struct db_wbxng_dummy *db = (struct db_wbxng_dummy *) dbb;

  int pins;
  pins = hal_gpio_read( db->base.is_tx ? GPIO_TX_BANK : GPIO_RX_BANK );
  if(pins & PLL_LOCK_DETECT)
    //printf("Got Locked Status from Synth");
    return true;

  //printf("Got Unlocked Status from Synth");
  return false;
}

