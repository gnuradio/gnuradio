/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_HAL_IO_H
#define INCLUDED_HAL_IO_H

#include "memory_map.h"

void hal_io_init(void);
void hal_finish();


/*
 * ------------------------------------------------------------------------
 * The GPIO pins are organized into two banks of 16-bits.
 * Bank 0 goes to the Tx daughterboard, Bank 1 goes to the Rx daughterboard.
 *
 * Each pin may be configured as an input or an output from the FPGA.
 * For output pins, there are four signals which may be routed to the
 * pin.  The four signals are the value written by s/w, the output of
 * the ATR controller, or two different sources of debug info from the
 * FPGA fabric.
 * ------------------------------------------------------------------------
 */

#define GPIO_TX_BANK	0	// pins that connect to the Tx daughterboard
#define	GPIO_RX_BANK	1	// pins that connect to the Rx daughterboard


/*!
 * \brief Set the data direction for GPIO pins
 *
 * If the bit is set, it's an output from the FPGA.
 * \param value is a 16-bit bitmask of values
 * \param mask is a 16-bit bitmask of which bits to effect.
 */ 
void hal_gpio_set_ddr(int bank, int value, int mask);

/*!
 * \brief Select the source of the signal for an output pin.
 *
 * \param code is is one of 's', 'a', '0', '1'
 *   where 's' selects software output, 'a' selects ATR output, '0' selects
 *   debug 0, '1' selects debug 1.
 */
void hal_gpio_set_sel(int bank, int bitno, char code);

/*!
 * \brief Select the source of the signal for the output pins.
 *
 * \param codes is is a string of 16 characters composed of '.', 's',
 * 'a', '0', or '1' where '.' means "don't change", 's' selects
 * software output, 'a' selects ATR output, '0' selects debug 0, '1'
 * selects debug 1.
 */
void hal_gpio_set_sels(int bank, char *codes);


/*!
 * \brief write \p value to gpio pins specified by \p mask.
 */
void hal_gpio_write(int bank, int value, int mask);

/*!
 * \brief read GPIO bits
 */
int  hal_gpio_read(int bank);


/*
 * ------------------------------------------------------------------------
 *			   control the leds
 *
 * Low 4-bits are the general purpose leds on the board
 * The next bit is the led on the ethernet connector
 * ------------------------------------------------------------------------
 */

void hal_set_leds(int value, int mask);
void hal_set_led_src(int value, int mask);
void hal_toggle_leds(int mask);

/*
 * ------------------------------------------------------------------------
 *			   simple timeouts
 * ------------------------------------------------------------------------
 */



static inline void
hal_set_timeout(int delta_ticks)
{
  int t = timer_regs->time + delta_ticks;
  if (t == 0)			// kills timer
    t = 1;
  timer_regs->time = t;
}

/*
 * ------------------------------------------------------------------------
 *			interrupt enable/disable
 * ------------------------------------------------------------------------
 */

/*!
 * \brief Disable interrupts and return previous interrupt enable state.
 * [Microblaze specific]
 */
static inline int
hal_disable_ints(void)
{
  int result, t0;

  asm volatile("mfs   %0, rmsr       \n\
		andni %1, %0, 0x2    \n\
		mts   rmsr, %1"
	       : "=r" (result), "=r" (t0));
  return result;
}

/*!
 * \brief Enable interrupts and return previous interrupt enable state.
 * [Microblaze specific]
 */
static inline int
hal_enable_ints(void)
{
  int result, t0;

  asm volatile("mfs  %0, rmsr	      \n\
		ori  %1, %0, 0x2      \n\
		mts  rmsr, %1"
	       : "=r" (result), "=r" (t0));
  return result;
}

/*!
 * \brief Set interrupt enable state to \p prev_state.
 * [Microblaze specific]
 */
static inline void
hal_restore_ints(int prev_state)
{
  int t0, t1;
  asm volatile("andi  %0, %2, 0x2	\n\
		mfs   %1, rmsr          \n\
		andni %1, %1, 0x2	\n\
		or    %1, %1, %0	\n\
		mts   rmsr, %1"
	       : "=r" (t0), "=r"(t1) : "r" (prev_state));
}

#endif /* INCLUDED_HAL_IO_H */
