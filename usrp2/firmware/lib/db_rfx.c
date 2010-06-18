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

#include <spi.h>
#include <memory_map.h>
#include <db_base.h>
#include <hal_io.h>
#include <stdio.h>
#include <mdelay.h>
#include <lsdac.h>
#include <clocks.h>


bool rfx_init_rx(struct db_base *db);
bool rfx_init_tx(struct db_base *db);
bool rfx_set_freq(struct db_base *db, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc);
bool rfx_set_gain_rx(struct db_base *db, u2_fxpt_gain_t gain);
bool rfx_set_gain_tx(struct db_base *db, u2_fxpt_gain_t gain);
bool rfx_set_tx_enable(struct db_base *, bool on);

// Control Latch Defines
#define P 0  // Prescalar value for setting in regs, must match the next line...
#define PRESCALER 8  // Presacalar value for computations
#define PD 0  // Power down, 0 = normal operation
#define PL 0 // PLL power output
#define MTLD 1 // Mute till lock detect
#define CPGAIN 0 // Charge pump gain, use setting 1, also in N-reg
#define CP3S 0 // Charge pump tri-state, 0 = normal operation
#define PDP 1 // Phase detector polarity
#define MUXOUT 1 // Digital lock detect, active high
#define CR 0 // normal operation
#define PC 1 // core power

// N Latch Defines
#define DIVSEL 0 // N Counter always operates on full rate
#define N_RSV 0

// R Latch Defines
#define R_RSV 0
#define R_BSC 3
#define R_TMB 0
#define R_LDP 1
#define R_ABP 0
#define R_DIV 16

#define phdet_freq (U2_DOUBLE_TO_FXPT_FREQ(100e6/R_DIV))

// IO Pin functions
#define POWER_UP  (1 << 7)  //  Low enables power supply
#define ANT_SW  (1 << 6)   // On TX DB, 0 = TX, 1 = RX, on RX DB 0 = main ant, 1 = RX2
#define MIX_EN  (1 << 5)   // Enable appropriate mixer
#define LOCKDET_MASK (1 << 2)   // Input pin

struct db_rfx_common {
  // RFX common stuff
  unsigned char DIV2;
  unsigned char CP1;
  unsigned char CP2;
  int freq_mult;
  int spi_mask;  
};

struct db_rfx_dummy {
  struct db_base	base;
  struct db_rfx_common	common;
};


struct db_rfx_400_rx {
  struct db_base	base;
  struct db_rfx_common	common;
};

struct db_rfx_400_tx {
  struct db_base	base;
  struct db_rfx_common	common;
};

struct db_rfx_900_rx {
  struct db_base	base;
  struct db_rfx_common	common;
};

struct db_rfx_900_tx {
  struct db_base	base;
  struct db_rfx_common	common;
};

struct db_rfx_1200_rx {
  struct db_base	base;
  struct db_rfx_common	common;
};

struct db_rfx_1200_tx {
  struct db_base	base;
  struct db_rfx_common	common;
};

struct db_rfx_1800_rx {
  struct db_base	base;
  struct db_rfx_common	common;
};

struct db_rfx_1800_tx {
  struct db_base	base;
  struct db_rfx_common	common;
};

struct db_rfx_2200_rx {
  struct db_base	base;
  struct db_rfx_common	common;
};

struct db_rfx_2200_tx {
  struct db_base	base;
  struct db_rfx_common	common;
};

struct db_rfx_2400_rx {
  struct db_base	base;
  struct db_rfx_common	common;
};

struct db_rfx_2400_tx {
  struct db_base	base;
  struct db_rfx_common	common;
};


/*
 * The class instances
 */
struct db_rfx_400_rx db_rfx_400_rx = {
  .base.dbid = 0x0024,
  .base.is_tx = false,
  .base.output_enables = 0x00E0,
  .base.used_pins = 0x00FF,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(400e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(500e6),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(45),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(0.022),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = true,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.init = rfx_init_rx,
  .base.set_freq = rfx_set_freq,
  .base.set_gain = rfx_set_gain_rx,
  .base.set_tx_enable = 0,
  .base.atr_mask = 0x00E0,
  .base.atr_txval = POWER_UP,
  .base.atr_rxval = POWER_UP|MIX_EN,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.DIV2 = 0,
  .common.CP1 = 7,
  .common.CP2 = 7,
  .common.spi_mask = SPI_SS_RX_DB,
  .common.freq_mult = 2
};


struct db_rfx_400_tx db_rfx_400_tx = {
  .base.dbid = 0x0028,
  .base.is_tx = true,
  .base.output_enables = 0x00E0,
  .base.used_pins = 0x00FF,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(400e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(500e6),
  //.base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  //.base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  //.base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(12.5e6),
  .base.init = rfx_init_tx,
  .base.set_freq = rfx_set_freq,
  .base.set_gain = rfx_set_gain_tx,
  .base.set_tx_enable = rfx_set_tx_enable,
  .base.atr_mask = 0x00E0,
  .base.atr_txval = POWER_UP|MIX_EN, 
  .base.atr_rxval = POWER_UP|ANT_SW,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.DIV2 = 1,
  .common.CP1 = 7,
  .common.CP2 = 7,
  .common.spi_mask = SPI_SS_TX_DB,
  .common.freq_mult = 2
};

struct db_rfx_900_rx db_rfx_900_rx = {
  .base.dbid = 0x0025,
  .base.is_tx = false,
  .base.output_enables = 0x00E0,
  .base.used_pins = 0x00FF,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(750e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(1050e6),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(70),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(0.034),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = true,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.init = rfx_init_rx,
  .base.set_freq = rfx_set_freq,
  .base.set_gain = rfx_set_gain_rx,
  .base.set_tx_enable = 0,
  .base.atr_mask = 0x00E0,
  .base.atr_txval = 0,
  .base.atr_rxval = MIX_EN,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.DIV2 = 1,
  .common.CP1 = 7,
  .common.CP2 = 7,
  .common.spi_mask = SPI_SS_RX_DB,
  .common.freq_mult = 2
};


struct db_rfx_900_tx db_rfx_900_tx = {
  .base.dbid = 0x0029,
  .base.is_tx = true,
  .base.output_enables = 0x00E0,
  .base.used_pins = 0x00FF,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(750e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(1050e6),
  //.base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  //.base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  //.base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(12.5e6),
  .base.init = rfx_init_tx,
  .base.set_freq = rfx_set_freq,
  .base.set_gain = rfx_set_gain_tx,
  .base.set_tx_enable = rfx_set_tx_enable,
  .base.atr_mask = 0x00E0,
  .base.atr_txval = MIX_EN, 
  .base.atr_rxval = ANT_SW,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.DIV2 = 1,
  .common.CP1 = 7,
  .common.CP2 = 7,
  .common.spi_mask = SPI_SS_TX_DB,
  .common.freq_mult = 2
};

struct db_rfx_1200_rx db_rfx_1200_rx = {
  .base.dbid = 0x0026,
  .base.is_tx = false,
  .base.output_enables = 0x00E0,
  .base.used_pins = 0x00FF,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(1150e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(1450e6),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(70),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(0.034),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = true,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.init = rfx_init_rx,
  .base.set_freq = rfx_set_freq,
  .base.set_gain = rfx_set_gain_rx,
  .base.set_tx_enable = 0,
  .base.atr_mask = 0x00E0,
  .base.atr_txval = 0,
  .base.atr_rxval = MIX_EN,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.DIV2 = 1,
  .common.CP1 = 7,
  .common.CP2 = 7,
  .common.spi_mask = SPI_SS_RX_DB,
  .common.freq_mult = 2
};


struct db_rfx_1200_tx db_rfx_1200_tx = {
  .base.dbid = 0x002a,
  .base.is_tx = true,
  .base.output_enables = 0x00E0,
  .base.used_pins = 0x00FF,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(1150e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(1450e6),
  //.base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  //.base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  //.base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(12.5e6),
  .base.init = rfx_init_tx,
  .base.set_freq = rfx_set_freq,
  .base.set_gain = rfx_set_gain_tx,
  .base.set_tx_enable = rfx_set_tx_enable,
  .base.atr_mask = 0x00E0,
  .base.atr_txval = MIX_EN, 
  .base.atr_rxval = ANT_SW,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.DIV2 = 1,
  .common.CP1 = 7,
  .common.CP2 = 7,
  .common.spi_mask = SPI_SS_TX_DB,
  .common.freq_mult = 2
};

struct db_rfx_1800_rx db_rfx_1800_rx = {
  .base.dbid = 0x0034,
  .base.is_tx = false,
  .base.output_enables = 0x00E0,
  .base.used_pins = 0x00FF,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(1500e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(2100e6),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(70),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(0.034),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = true,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.init = rfx_init_rx,
  .base.set_freq = rfx_set_freq,
  .base.set_gain = rfx_set_gain_rx,
  .base.set_tx_enable = 0,
  .base.atr_mask = 0x00E0,
  .base.atr_txval = 0,
  .base.atr_rxval = MIX_EN,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.DIV2 = 0,
  .common.CP1 = 7,
  .common.CP2 = 7,
  .common.spi_mask = SPI_SS_RX_DB,
  .common.freq_mult = 1
};


struct db_rfx_1800_tx db_rfx_1800_tx = {
  .base.dbid = 0x0035,
  .base.is_tx = true,
  .base.output_enables = 0x00E0,
  .base.used_pins = 0x00FF,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(1500e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(2100e6),
  //.base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  //.base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  //.base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(12.5e6),
  .base.init = rfx_init_tx,
  .base.set_freq = rfx_set_freq,
  .base.set_gain = rfx_set_gain_tx,
  .base.set_tx_enable = rfx_set_tx_enable,
  .base.atr_mask = 0x00E0,
  .base.atr_txval = MIX_EN, 
  .base.atr_rxval = ANT_SW,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.DIV2 = 0,
  .common.CP1 = 7,
  .common.CP2 = 7,
  .common.spi_mask = SPI_SS_TX_DB,
  .common.freq_mult = 1  
};


struct db_rfx_2200_rx db_rfx_2200_rx = {
  .base.dbid = 0x002c,
  .base.is_tx = false,
  .base.output_enables = 0x00E0,
  .base.used_pins = 0x00FF,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(2000e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(2400e6),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(70),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(0.034),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = true,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.init = rfx_init_rx,
  .base.set_freq = rfx_set_freq,
  .base.set_gain = rfx_set_gain_rx,
  .base.set_tx_enable = 0,
  .base.atr_mask = 0x00E0,
  .base.atr_txval = 0,
  .base.atr_rxval = MIX_EN,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.DIV2 = 0,
  .common.CP1 = 7,
  .common.CP2 = 7,
  .common.spi_mask = SPI_SS_RX_DB,
  .common.freq_mult = 1
};


struct db_rfx_2200_tx db_rfx_2200_tx = {
  .base.dbid = 0x002d,
  .base.is_tx = true,
  .base.output_enables = 0x00E0,
  .base.used_pins = 0x00FF,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(2000e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(2400e6),
  //.base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  //.base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  //.base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(12.5e6),
  .base.init = rfx_init_tx,
  .base.set_freq = rfx_set_freq,
  .base.set_gain = rfx_set_gain_tx,
  .base.set_tx_enable = rfx_set_tx_enable,
  .base.atr_mask = 0x00E0,
  .base.atr_txval = MIX_EN, 
  .base.atr_rxval = ANT_SW,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.DIV2 = 0,
  .common.CP1 = 7,
  .common.CP2 = 7,
  .common.spi_mask = SPI_SS_TX_DB,
  .common.freq_mult = 1
};


struct db_rfx_2400_rx db_rfx_2400_rx = {
  .base.dbid = 0x0027,
  .base.is_tx = false,
  .base.output_enables = 0x00E0,
  .base.used_pins = 0x00FF,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(2300e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(2900e6),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(70),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(0.034),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = true,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.init = rfx_init_rx,
  .base.set_freq = rfx_set_freq,
  .base.set_gain = rfx_set_gain_rx,
  .base.set_tx_enable = 0,
  .base.atr_mask = 0x00E0,
  .base.atr_txval = 0,
  .base.atr_rxval = MIX_EN,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.DIV2 = 0,
  .common.CP1 = 7,
  .common.CP2 = 7,
  .common.spi_mask = SPI_SS_RX_DB,
  .common.freq_mult = 1
};


struct db_rfx_2400_tx db_rfx_2400_tx = {
  .base.dbid = 0x002b,
  .base.is_tx = true,
  .base.output_enables = 0x00E0,
  .base.used_pins = 0x00FF,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(2300e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(2900e6),
  //.base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  //.base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  //.base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(xxx),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(12.5e6),
  .base.init = rfx_init_tx,
  .base.set_freq = rfx_set_freq,
  .base.set_gain = rfx_set_gain_tx,
  .base.set_tx_enable = rfx_set_tx_enable,
  .base.atr_mask = 0x00E0,
  .base.atr_txval = MIX_EN, 
  .base.atr_rxval = ANT_SW,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .base.set_antenna = 0,
  .common.DIV2 = 0,
  .common.CP1 = 7,
  .common.CP2 = 7,
  .common.spi_mask = SPI_SS_TX_DB,
  .common.freq_mult = 1
};


bool
rfx_init_tx(struct db_base *dbb)
{
  //struct db_rfx_dummy *db = (struct db_rfx_dummy *) dbb;
  clocks_enable_tx_dboard(true, 0);

  // Set the freq now to get the one time 10ms delay out of the way.
  u2_fxpt_freq_t	dc;
  dbb->set_freq(dbb, dbb->freq_min, &dc);
  return true;
}

bool
rfx_init_rx(struct db_base *dbb)
{
  //struct db_rfx_dummy *db = (struct db_rfx_dummy *) dbb;
  clocks_enable_rx_dboard(true, 0);

  // test gain
  dbb->set_gain(dbb,U2_DOUBLE_TO_FXPT_GAIN(45.0));

  // Set the freq now to get the one time 10ms delay out of the way.
  u2_fxpt_freq_t	dc;
  dbb->set_freq(dbb, dbb->freq_min, &dc);

  return true;
}

bool
rfx_set_freq(struct db_base *dbb, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc)
{
  static unsigned char first = true;

  *dc = 0;
  struct db_rfx_dummy *db = (struct db_rfx_dummy *) dbb;
  u2_fxpt_freq_t desired_n = (U2_DOUBLE_TO_FXPT_FREQ(1.0)*db->common.freq_mult*freq)/phdet_freq;
  int N_DIV = u2_fxpt_freq_round_to_int(desired_n);
  int B = N_DIV/PRESCALER;
  int A = N_DIV - PRESCALER*B;

  if(B<A)
    return false;

  int R = (R_RSV<<22)|(R_BSC<<20)|(R_TMB<<19)|(R_LDP<<18)|(R_ABP<<16)|(R_DIV<<2)|1;
  int N = (DIVSEL<<23)|(db->common.DIV2<<22)|(CPGAIN<<21)|(B<<8)|(N_RSV<<7)|(A<<2)|2;
  int C = (P<<22)|(PD<<20)|(db->common.CP2<<17)|(db->common.CP1<<14)|(PL<<12)|
    (MTLD<<11)|(CPGAIN<<10)|(CP3S<<9)|(PDP<<8)|(MUXOUT<<5)|(CR<<4)|(PC<<2)|0;

  spi_transact(SPI_TXONLY,db->common.spi_mask,R,24,SPIF_PUSH_FALL);
  spi_transact(SPI_TXONLY,db->common.spi_mask,C,24,SPIF_PUSH_FALL);
  if (first){
    first = false;
    mdelay(10);
  }
  spi_transact(SPI_TXONLY,db->common.spi_mask,N,24,SPIF_PUSH_FALL);

  //printf("A = %d, B = %d, N_DIV = %d\n",A, B, N_DIV);
  *dc = (N_DIV * phdet_freq) / db->common.freq_mult;
  return true;
}

bool
rfx_set_gain_tx(struct db_base *dbb, u2_fxpt_gain_t gain)
{
  // There is no analog gain control on TX
  return true;
}

bool
rfx_set_gain_rx(struct db_base *dbb, u2_fxpt_gain_t gain)
{
  struct db_rfx_dummy *db = (struct db_rfx_dummy *) dbb;

  int offset_q8 = (int)(1.2/3.3*4096*(1<<15));  
  int range_q15 = (int)(-1.0*4096/3.3*256*128);
  int slope_q8 = range_q15/db->base.gain_max;

  int dacword = ((slope_q8 * gain) + offset_q8)>>15;
  //printf("DACWORD %d\n",dacword);
  lsdac_write_rx(1,dacword);
  return true;
}


bool
rfx_set_tx_enable(struct db_base *dbb, bool on)
{
  struct db_rfx_dummy *db = (struct db_rfx_dummy *) dbb;

  // FIXME

  return false;
}

bool
rfx_lock_detect(struct db_base *dbb)
{
  struct db_rfx_dummy *db = (struct db_rfx_dummy *) dbb;
  int pins;
  pins = hal_gpio_read( db->base.is_tx ? GPIO_TX_BANK : GPIO_RX_BANK );
  if(pins & LOCKDET_MASK)
    return true;
  return false;
}

/* 
    def select_rx_antenna(self, which_antenna):
        """
        Specify which antenna port to use for reception.
        @param which_antenna: either 'TX/RX' or 'RX2'
        """
        if which_antenna in (0, 'TX/RX'):
            self._u.write_io(self._which, 0,        RX2_RX1N)
        elif which_antenna in (1, 'RX2'):
            self._u.write_io(self._which, RX2_RX1N, RX2_RX1N)
        else:
            raise ValueError, "which_antenna must be either 'TX/RX' or 'RX2'"

    def set_gain(self, gain):
        """
        Set the gain.

        @param gain:  gain in decibels
        @returns True/False
        """
        maxgain = self.gain_range()[1] - self._u.pga_max()
        mingain = self.gain_range()[0]
        if gain > maxgain:
            pga_gain = gain-maxgain
            assert pga_gain <= self._u.pga_max()
            agc_gain = maxgain
        else:
            pga_gain = 0
            agc_gain = gain
        V_maxgain = .2
        V_mingain = 1.2
        V_fullscale = 3.3
        dac_value = (agc_gain*(V_maxgain-V_mingain)/(maxgain-mingain) + V_mingain)*4096/V_fullscale
        assert dac_value>=0 and dac_value<4096
        return self._u.write_aux_dac(self._which, 0, int(dac_value)) and \
               self._set_pga(int(pga_gain))

    def gain_range(self):
        return (self._u.pga_min(), self._u.pga_max() + 70, 0.05) -- For 900-2400
        return (self._u.pga_min(), self._u.pga_max() + 45, 0.035) -- For 400

*/
