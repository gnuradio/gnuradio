/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_CLOCKS_H
#define INCLUDED_CLOCKS_H

/*
 * Routines to configure our multitude of clocks
 */

#include <bool.h>
#include <usrp2_mimo_config.h>


/*!
 * One time call to initialize all clocks to a reasonable state.  We
 * come out of here using our free running 100MHz oscilator and not
 * providing a clock to the MIMO connector (CMC_WE_DONT_LOCK)
 */
void clocks_init(void);


/*!
 * \brief MIMO clock configuration.
 *
 * Configure our master clock source, and whether or not we drive a
 * clock onto the mimo connector.  See MC_flags in usrp2_mimo_config.h.
 */
void clocks_mimo_config(int flags);

/*!
 * \brief Lock Detect -- Return True if our PLL is locked
 */
bool clocks_lock_detect();

/*!
 * \brief Enable or disable test clock (extra clock signal)
 */
void clocks_enable_test_clk(bool enable, int divisor);

/*!
 * \brief Enable or disable fpga clock.  Disabling would wedge and require a power cycle.
 */
void clocks_enable_fpga_clk(bool enable, int divisor);

/*!
 * \brief Enable or disable clock output sent to MIMO connector
 */
void clocks_enable_clkexp_out(bool enable, int divisor);

/*!
 * \brief Enable or disable ethernet phyclk, should always be disabled
 */
void clocks_enable_eth_phyclk(bool enable, int divisor);

/*!
 * \brief Enable or disable clock to DAC
 */
void clocks_enable_dac_clk(bool enable, int divisor);

/*!
 * \brief Enable or disable clock to ADC
 */
void clocks_enable_adc_clk(bool enable, int divisor);

/*!
 * \brief Enable or disable clock to Rx daughterboard
 */
void clocks_enable_rx_dboard(bool enable, int divisor);


/*!
 * \brief Enable or disable clock to Tx daughterboard
 */
void clocks_enable_tx_dboard(bool enable, int divisor);


#endif /* INCLUDED_CLOCKS_H */
