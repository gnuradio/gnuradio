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

#ifndef DB_WBXNG_H
#define DB_WBXNG_H

#include <db_base.h>

// IO Pin functions
// Tx and Rx have shared defs, but different i/o regs
#define ENABLE_5        (1 << 7)              // enables 5.0V power supply
#define ENABLE_33       (1 << 6)              // enables 3.3V supply
//#define RX_TXN          (1 << 15)             // Tx only: T/R antenna switch for TX/RX port
//#define RX2_RX1N        (1 << 15)             // Rx only: antenna switch between RX2 and TX/RX port
#define RX_TXN          ((1 << 5)|(1 << 15))  // Tx only: T/R antenna switch for TX/RX port
#define RX2_RX1N        ((1 << 5)|(1 << 15))  // Rx only: antenna switch between RX2 and TX/RX port
#define RXBB_EN         (1 << 4)
#define TXMOD_EN        (1 << 4)
#define PLL_CE          (1 << 3)
#define PLL_PDBRF       (1 << 2)
#define PLL_MUXOUT      (1 << 1)
#define PLL_LOCK_DETECT (1 << 0)

// RX Attenuator constants
#define ATTN_SHIFT	8
#define ATTN_MASK	(63 << ATTN_SHIFT)

struct db_wbxng_common {
  // RFX common stuff
  uint16_t adf4350_regs_int;
  uint16_t adf4350_regs_frac;
  uint8_t adf4350_regs_prescaler;
  uint16_t adf4350_regs_mod;
  uint16_t adf4350_regs_10_bit_r_counter;
  uint8_t adf4350_regs_divider_select;
  uint8_t adf4350_regs_8_bit_band_select_clock_divider_value;

  int freq_mult;
  int spi_mask;
};

struct db_wbxng_dummy {
  struct db_base	base;
  struct db_wbxng_common	common;
};


struct db_wbxng_rx {
  struct db_base	base;
  struct db_wbxng_common	common;
};

struct db_wbxng_tx {
  struct db_base	base;
  struct db_wbxng_common	common;
};


#endif /* DB_WBXNG_H */
