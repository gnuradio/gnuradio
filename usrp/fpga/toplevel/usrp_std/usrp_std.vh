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

// ====================================================================
//            User control over what parts get included
//
//                  >>>> EDIT ONLY THIS SECTION <<<<
//
// ====================================================================

// Uncomment this for 1 rx channel (w/ halfband) & 1 transmit channel
//`include "usrp_std_config_1rxhb_1tx.vh"

// Uncomment this for 2 rx channels (w/ halfband) & 2 transmit channels
  `include "usrp_std_config_2rxhb_2tx.vh"

// Uncomment this for 4 rx channels (w/o halfband) & 0 transmit channels
//`include "usrp_std_config_4rx_0tx.vh"

// Add other "known to fit" configurations here...

// ====================================================================
// 
//                  >>>> DO NOT EDIT BELOW HERE <<<<
//
// [The stuff from here down is derived from the stuff included above]
//
// N.B., *all* the remainder of the code should be conditionalized
// only in terms of:
//
//  TX_ON, TX_EN_0, TX_EN_1, TX_EN_2, TX_EN_3, TX_CAP_NCHAN, TX_CAP_HB,
//  RX_ON, RX_EN_0, RX_EN_1, RX_EN_2, RX_EN_3, RX_CAP_NCHAN, RX_CAP_HB,
//  RX_NCO_ON, RX_CIC_ON
// ====================================================================

`ifdef TX_ON

 `ifdef TX_SINGLE
  `define TX_EN_0
  `define TX_CAP_NCHAN 3'd1
 `endif

 `ifdef TX_DUAL
  `define TX_EN_0
  `define TX_EN_1
  `define TX_CAP_NCHAN 3'd2
 `endif

 `ifdef TX_QUAD
  `define TX_EN_0
  `define TX_EN_1
  `define TX_EN_2
  `define TX_EN_3
  `define TX_CAP_NCHAN 3'd4
 `endif

 `ifdef TX_HB_ON
  `define TX_CAP_HB   1
 `else
  `define TX_CAP_HB   0
 `endif

`else	// !ifdef TX_ON

 `define TX_CAP_NCHAN 3'd0
 `define TX_CAP_HB 0

`endif // !ifdef TX_ON

// --------------------------------------------------------------------

`ifdef RX_ON

 `ifdef RX_SINGLE
  `define RX_EN_0
  `define RX_CAP_NCHAN 3'd1
 `endif

 `ifdef RX_DUAL
  `define RX_EN_0
  `define RX_EN_1
  `define RX_CAP_NCHAN 3'd2
 `endif

 `ifdef RX_QUAD
  `define RX_EN_0
  `define RX_EN_1
  `define RX_EN_2
  `define RX_EN_3
  `define RX_CAP_NCHAN 3'd4
 `endif

 `ifdef RX_HB_ON
  `define RX_CAP_HB   1
 `else
  `define RX_CAP_HB   0
 `endif

`else	// !ifdef RX_ON

 `define RX_CAP_NCHAN 3'd0
 `define RX_CAP_HB 0

`endif // !ifdef RX_ON
