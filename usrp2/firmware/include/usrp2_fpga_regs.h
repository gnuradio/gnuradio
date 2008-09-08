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

#ifndef INCLUDED_USRP2_FPGA_REGS_H
#define INCLUDED_USSRP2_FPGA_REGS_H

#include "usrp2_cdefs.h"

__U2_BEGIN_DECLS

// ----------------------------------------------------------------

#define	DSP_CORE_TX_BASE	128

// DUC center frequency tuning word (phase increment)
#define FR_TX_FREQ_0		(0 + DSP_CORE_TX_BASE)

// I & Q output scaling, 16.0 format ((I_SCALE << 16) | Q_SCALE)
#define FR_TX_SCALE_0		(1 + DSP_CORE_TX_BASE)

// Tx interpolation rate (set to 1 less than desired rate)
#define FR_TX_INTERP_RATE_0	(2 + DSP_CORE_TX_BASE)

// Write 1 (actually anything) to clear tx state
#define FR_TX_CLEAR_STATE_0	(3 + DSP_CORE_TX_BASE)

// ----------------------------------------------------------------

#define	DSP_CORE_RX_BASE	160

// DDC center frequency tuning word (phase increment)
#define FR_RX_FREQ_0		(0 + DSP_CORE_RX_BASE)

// I & Q input scaling, 16.0 format ((I_SCALE << 16) | Q_SCALE)
#define FR_RX_SCALE_0		(1 + DSP_CORE_RX_BASE)

// Rx decimation rate (set to 1 less than desired rate)
#define FR_RX_DECIM_RATE_0	(2 + DSP_CORE_RX_BASE)

// The next two registers concatenated are the Rx command register.
//
// Writing FR_RX_TIME_TO_RX_0 writes the concatenated value into the
// cmd queue.  Thus, if you're writing both, be sure to write
// FR_RX_QTY_0 first.
//
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                          Timestamp                            |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define FR_RX_TIME_TO_RX	(3 + DSP_CORE_RX_BASE)

//                          23-bits                       9-bits
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                number_of_lines              | lines_per_frame |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define FR_RX_QTY_0		(4 + DSP_CORE_RX_BASE)

// write a 1 (anything actually) to clear the overrun 
#define FR_RX_CLR_OVERRUN_0	(5 + DSP_CORE_RX_BASE)


__U2_END_DECLS

#endif /* INCLUDED_USRP2_FPGA_REGS_H */
