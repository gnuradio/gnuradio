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

#ifndef INCLUDED_DB_BASE_H
#define INCLUDED_DB_BASE_H

#include <usrp2_types.h>
#include <bool.h>

/*!
 * \brief "base class" for firmware version of daughterboard code
 */
struct db_base {
  uint16_t		dbid;		//< daughterboard ID

  uint16_t              is_tx 		  : 1;  //< is this a transmit db?
  uint16_t		is_quadrature 	  : 1;
  uint16_t		i_and_q_swapped   : 1;
  uint16_t		spectrum_inverted : 1;

  uint16_t		output_enables;	//< bitmask of which pins should be outputs from FPGA
  uint16_t		used_pins;	//< bitmask of pins used by the daughterboard

  u2_fxpt_freq_t	freq_min;	//< min freq that can be set (Hz)
  u2_fxpt_freq_t	freq_max;	//< max freq that can be set (Hz)

  u2_fxpt_gain_t	gain_min;	//< min gain that can be set (dB)
  u2_fxpt_gain_t	gain_max;	//< max gain that can be set (dB)
  u2_fxpt_gain_t	gain_step_size;	//< (dB)

  u2_fxpt_freq_t	default_lo_offset;  //< offset to add to tune frequency, reset value
  u2_fxpt_freq_t        current_lo_offset;  //< current value of lo_offset

  /*
   * Auto T/R control values
   */
  uint32_t		atr_mask;       //< which bits to control
  uint32_t		atr_txval;	//< value to use when transmitting
  uint32_t		atr_rxval;	//< value to use when receiving

  //! delay in clk ticks from when Tx fifo gets data to when T/R switches
  // uint32_t		atr_tx_delay;

  //! delay in clk ticks from when Tx fifo goes empty to when T/R switches
  // uint32_t		atr_rx_delay;

  bool	(*init)(struct db_base *);
  bool	(*set_freq)(struct db_base *, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc);
  bool	(*set_gain)(struct db_base *, u2_fxpt_gain_t gain);
  bool	(*set_tx_enable)(struct db_base *, bool on);
  bool	(*set_antenna)(struct db_base *, int ant);
};


#endif /* INCLUDED_DB_BASE_H */
