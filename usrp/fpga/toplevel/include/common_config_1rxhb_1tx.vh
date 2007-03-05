// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2006 Matt Ettus
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Boston, MA  02110-1301  USA
//

// ------------------------------------------------------------
// If TX_ON is not defined, there is *no* transmit circuitry built
  `define TX_ON

// ------------------------------------------------------------
// Define 1 and only one of TX_SINGLE, TX_DUAL and TX_QUAD
// to respectively enable 1, 2 or 4 transmit channels.
// [Please note that only TX_SINGLE and TX_DUAL are currently valid]
  `define TX_SINGLE
//`define TX_DUAL
//`define TX_QUAD

// ------------------------------------------------------------
// Define TX_HB_ON to enable the transmit halfband filter
// [Not implemented]
//`define TX_HB_ON

// ------------------------------------------------------------
// IF RX_ON is not defined, there is *no* receive circuitry built
  `define RX_ON

// ------------------------------------------------------------
// Define 1 and only one of RX_SINGLE, RX_DUAL and RX_QUAD
// to respectively define 1, 2 or 4 receive channels.

  `define RX_SINGLE
//`define RX_DUAL
//`define RX_QUAD

// ------------------------------------------------------------
// Define RX_HB_ON to enable the receive halfband filter
  `define RX_HB_ON

// ------------------------------------------------------------
// Define RX_NCO_ON to enable the receive Numerical Controlled Osc
  `define RX_NCO_ON

// ------------------------------------------------------------
// Define RX_CIC_ON to enable the receive Cascaded Integrator Comb filter
  `define RX_CIC_ON
